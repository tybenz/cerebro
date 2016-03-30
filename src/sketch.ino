#include <LightGrid.h>
#include <Storage.h>
#include <Model.h>
#include <Buttons.h>
#include <Preset.h>
#include <SoftwareSerial.h>

LightGrid* lightgrid;
Storage* storage = new Storage();
Model* model = new Model();
Model* oldModel;
Buttons* buttons = new Buttons();

SoftwareSerial mySerial(8, 7); // RX, TX

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#define LIVE 0
#define MIDI 1
#define PRESET 2
#define LOOPER 3
#define COPYSWAPSAVE 4
#define COPYSWAPSAVEWAIT 5
#define BANKSEARCH 6
#define SAVETYPE_NONE -1
#define SAVETYPE_LOOPS 0
#define SAVETYPE_MIDI 1
#define BUTTON_COUNT 10
#define ACTUAL_BUTTON_COUNT 6

int prevMode = 0;
int mode = 0;
bool firstLoop = true;

int modeButtonPin = 0;
int button1Pin    = 1;
int button2Pin    = 2;
int button3Pin    = 3;
int button4Pin    = 4;
int button5Pin    = 5;

long copySwapSaveStart = -1;
int lastPresetNum;
int srcPresetNum;
int targetPresetNum;
int saveType;
unsigned char saveLoops;
int saveMidi1;
int saveMidi2;
Preset *srcPreset;
Preset *targetPreset;

bool press[BUTTON_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool pressed[BUTTON_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool pressHold[BUTTON_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool release[BUTTON_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int modeColors[7][3] = {
    // LIVE red
    {255, 0, 0},
    // MIDI blue
    {0, 0, 255},
    // PRESET green
    {0, 255, 0},
    // LOOPER white
    {255, 255, 255},
    // COPYSWAPSAVE magenta
    {255, 50, 255},
    // COPYSWAPSAVEWAIT magenta
    {255, 50, 255},
    // BANKSEARCH green
    {0, 255, 0}
};

void sendMidi(int aMidiCommand, int aData1) {
    mySerial.write(aMidiCommand);
    mySerial.write(aData1);
    delay(100);
}

int writableMode = 0;

void setup() {
    mySerial.begin(31250);

    lightgrid = new LightGrid(3, 4, 5, 6);
    Serial.begin(9600);
    Serial.println(10000);
    pinMode(A5, INPUT_PULLUP); // sets analog pin for input

    mode = storage->getStartupMode();
    Serial.println(mode);

    unsigned char loops = storage->getStartupLoops();

    int midi1 = storage->getStartupMidi1();
    int midi2 = storage->getStartupMidi2();
    int preset = storage->getStartupPreset();

    // set model object
    int looper[3] = {0, 0, 0};
    model->setModel(preset, midi1, midi2, loops, looper);

    // initial render is taken care of in loop thanks to firstLoop
}

int debounceDelay = 30;
int oldInput = 0;
long lastReadTime = 0;

void loop() {
    int input = analogRead(A5);
    if (input != oldInput) {
        delay(20);
        int input2 = analogRead(A5);
        if (input - input2 < 20 && input - input2 > -20) {
            oldInput = input;
            int i = 0;
            oldModel = model->copy();

            buttons->updateStates(input);

            for (int i = 0; i < BUTTON_COUNT; i++) {
                press[i] = false;
                pressed[i] = false;
                pressHold[i] = false;
                release[i] = false;
            }
            buttons->detectEvents(press, pressed, pressHold, release);

            update();

            // If model has changed
            if (firstLoop || model->diff(oldModel) || prevMode != mode) {
                firstLoop = false;
                render();

                storage->saveModel(writableMode, model);
            }
            render();

            storage->saveModel(writableMode, model);
        }
    }
    oldInput = input;
}

void nextMode() {
    prevMode = mode;
    mode++;
    if (mode > 3) {
        mode = LIVE;
    }
    writableMode = mode;
}

void transition(int newMode) {
    prevMode = mode;
    if (newMode < 4) {
        writableMode = newMode;
    }
    mode = newMode;
}

void update() {
    // UPDATE MODEL
    if (press[0]) {
        // cycle through modes
        nextMode();
    }

    if (mode == LIVE) {
        // press on 1-5 triggers true bypass loop toggle
        if (press[1]) {
            model->toggleLoop(0);
        }
        if (press[2]) {
            model->toggleLoop(1);
        }
        if (press[3]) {
            model->toggleLoop(2);
        }
        if (press[4]) {
            model->toggleLoop(3);
        }
        if (press[5]) {
            model->toggleLoop(4);
        }
        if (pressHold[5]) {
            saveType = SAVETYPE_LOOPS;
            saveLoops = model->getLoops();
            transition(COPYSWAPSAVE);
        }
    } else if (mode == MIDI) {
        // press on 1 & 2 trigger midi1 down/up
        if (press[1]) {
            model->midi1Down();
        }
        if (press[2]) {
            model->midi1Up();
        }
        if (press[3]) {
            model->midi2Down();
        }
        if (press[4]) {
            model->midi2Up();
        }
        if (press[5]) {
            model->toggleLoop(4);
        }
        if(pressHold[5]) {
            // transition to SAVE
            saveType = SAVETYPE_MIDI;
            saveMidi1 = model->getMidi1();
            saveMidi2 = model->getMidi2();
            transition(COPYSWAPSAVE);
        }
    } else if (mode == PRESET) {
        // press on 1 & 2 trigger bank down/up. trigger BANKSEARCH
        int presetNum;
        Preset* preset;
        if (press[1]) {
            model->bankDown();
            transition(BANKSEARCH);
        }
        if (press[2]) {
            model->bankUp();
            transition(BANKSEARCH);
        }
        if (press[3]) {
            presetNum = model->selectPatch(0);
            preset = storage->getPresetByNum(presetNum);
            model->setLoops(preset->getLoops());
            model->setMidi1(preset->getMidi1());
            model->setMidi2(preset->getMidi2());
        }
        if (press[4]) {
            presetNum = model->selectPatch(1);
            preset = storage->getPresetByNum(presetNum);
            model->setLoops(preset->getLoops());
            model->setMidi1(preset->getMidi1());
            model->setMidi2(preset->getMidi2());
        }
        if (press[5]) {
            presetNum = model->selectPatch(2);
            preset = storage->getPresetByNum(presetNum);
            model->setLoops(preset->getLoops());
            model->setMidi1(preset->getMidi1());
            model->setMidi2(preset->getMidi2());
        }
        if (pressHold[3]) {
            srcPresetNum = model->getPresetNum(0);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold[4]) {
            srcPresetNum = model->getPresetNum(1);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold[5]) {
            srcPresetNum = model->getPresetNum(2);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
    } else if (mode == LOOPER) {
        // XXX should this go through model + render or be "special" in that we
        // can hard-code right here?
        //
        // press on 1, 2, 3 trigger relays
        if (press[1]) {
            model->activateLooperControl(0);
        }
        if (press[2]) {
            model->activateLooperControl(1);
        }
        if (press[3]) {
            model->activateLooperControl(2);
        }
        if (release[1]) {
            // set relay low
            model->deactivateLooperControl(0);
        }
        if (release[2]) {
            // set relay low
            model->deactivateLooperControl(1);
        }
        if (release[3]) {
            // set relay low
            model->deactivateLooperControl(2);
        }
    } else if (mode == COPYSWAPSAVE) {
        // when set, tempBank should be set to current bank
        if (press[1]) {
            model->bankDown();
        }
        if (press[2]) {
            model->bankUp();
        }
        if (pressHold[1] || pressHold[2]) {
            // model->selectPatchByNum(srcPresetNum);
            transition(prevMode);
        }
        if (press[3] || press[4] || press[5]) {
            if (press[3]) {
                targetPresetNum = model->getTempPresetNum(0);
            }
            if (press[4]) {
                targetPresetNum = model->getTempPresetNum(1);
            }
            if (press[5]) {
                targetPresetNum = model->getTempPresetNum(2);
            }
            targetPreset = storage->getPresetByNum(targetPresetNum);

            if (prevMode == PRESET) {
                // came from PRESET means i'm either swapping or copying
                storage->savePresetByNum(srcPreset, targetPresetNum);
                delete srcPreset;
                transition(COPYSWAPSAVEWAIT);
            } else if (saveType != SAVETYPE_NONE) {
                // came from either LIVE or MIDI, just saving to a preset
                if (saveType == SAVETYPE_LOOPS) {
                    storage->saveLoopsToPreset(saveLoops, targetPresetNum);
                    model->selectPatchByNum(targetPresetNum);
                } else if (saveType == SAVETYPE_MIDI) {
                    storage->saveMidiToPreset(saveMidi1, saveMidi2, targetPresetNum);
                    model->selectPatchByNum(targetPresetNum);
                }
                saveType = SAVETYPE_NONE;
                transition(COPYSWAPSAVEWAIT);
            }
        }
    } else if (mode == COPYSWAPSAVEWAIT) {
        // We wait to see if there is a press hold, which forces a swap
        if (copySwapSaveStart == -1) {
            copySwapSaveStart = millis();
        } else if (millis() - copySwapSaveStart > 2000) {
            model->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            targetPresetNum = -1;
            transition(PRESET);
            copySwapSaveStart = -1;
        }

        if (pressHold[3] || pressHold[4] || pressHold[5]) {
            if (srcPreset) {
                storage->savePresetByNum(targetPreset, srcPresetNum);
            }
            model->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            transition(PRESET);
            copySwapSaveStart = -1;
        }
    } else if (mode == BANKSEARCH) {
        if (press[1]) {
            model->bankDown();
        }
        if (press[2]) {
            model->bankUp();
        }
        if (pressHold[1] || pressHold[2]) {
            // in bank search, last preset from PRESET mode is still active
            // so just clear the temp bank and transition to preset
            model->clearTempBank();
            transition(PRESET);
        }
        if (press[3]) {
            model->selectPatch(0, true);
            transition(PRESET);
        }
        if (press[4]) {
            model->selectPatch(1, true);
            transition(PRESET);
        }
        if (press[5]) {
            model->selectPatch(2, true);
            transition(PRESET);
        }
    }
}

void render() {
    // Serial.println("RENDER");
    int* colors = modeColors[mode];
    int i;
    long on;
    if (copySwapSaveStart != -1) {
        on = (( millis() - copySwapSaveStart ) / 5) % 2;
    }

    if (mode != COPYSWAPSAVEWAIT || on) {
        lightgrid->setMode(colors[0], colors[1], colors[2]);
    } else {
        // flash on COPYSWAPSAVEWAIT
        lightgrid->setMode(0, 0, 0);
    }

    // lightgrid->turnOffAll();

    unsigned char loops;
    bool ledModels[9] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (mode == LIVE) {
        // light up which loops are active
        loops = model->loops;
        for (i = 0; i < 5; i++) {
            if((loops >> i) & 1) {
                ledModels[i] = true;
            }
        }
    } else if (mode == MIDI) {
        if (press[1] || pressed[1]) {
            ledModels[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledModels[1] = true;
        }
        if (press[3] || pressed[3]) {
            ledModels[2] = true;
        }
        if (press[4] || pressed[4]) {
            ledModels[3] = true;
        }
        // light up led5 if loop 5 is active
        loops = model->loops;
        if((loops >> 4) & 1) {
            ledModels[4] = true;
        }
    } else if (mode == PRESET) {
        if (press[1] || pressed[1]) {
            ledModels[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledModels[1] = true;
        }
        // light up bank leds
        int bank = model->getBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledModels[(3-i)+5] = true;
            }
        }
        ledModels[model->getPatch() + 2] = true;
    } else if (mode == BANKSEARCH) {
        if (press[1] || pressed[1]) {
            ledModels[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledModels[1] = true;
        }
        // light up bank leds
        int bank = model->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledModels[(3-i)+5] = true;
            }
        }
    } else if (mode == LOOPER) {
        if (press[1] || pressed[1]) {
            ledModels[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledModels[1] = true;
        }
        if (press[3] || pressed[3]) {
            ledModels[2] = true;
        }
    } else if (mode == COPYSWAPSAVE) {
        // light up bank leds
        int bank = model->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledModels[(3-i)+5] = true;
            }
        }
    } else if (mode == COPYSWAPSAVEWAIT) {
        // light up bank leds
        int bank = model->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledModels[(3-i)+5] = true;
            }
        }
        if (targetPresetNum != -1) {
            ledModels[model->getPatchForPresetNum(targetPresetNum) + 2] = true;
        }
    }
    lightgrid->setLeds(ledModels);

    lightgrid->writeShifter();

    int program;
    if (model->midi1 != oldModel->midi1) {
        // send midi program changes for midi1 and midi2
        program = model->getMidi1();
        // Serial.println("SEND MIDI 1");
        // Serial.println(program);
        sendMidi(0xC1, program);
    }

    if (model->midi2 != oldModel->midi2) {
        program = model->getMidi2();
        // Serial.println("SEND MIDI 2");
        // Serial.println(program);
        sendMidi(0xC0, program);
    }

    if (model->loops != oldModel->loops) {
        // trigger active loops, deactivate inactive loops
    }

    // set looper control relays to high if activated
    // set looper control relays to low if deactivated
    // set all looper controls to neutral in the model
}
