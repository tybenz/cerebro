#include <LightGrid.h>
#include <Storage.h>
#include <State.h>
#include <Buttons.h>
#include <Preset.h>
#include <SetlistPreset.h>
#include <SoftwareSerial.h>

LightGrid* lightgrid;
Storage* storage = new Storage();
State* state = new State();
Buttons* buttons = new Buttons();

SoftwareSerial mySerial(8, 7); // RX, TX

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#define LIVE 0
#define MIDI 1
#define PRESET 2
#define LOOPER 3
#define ABLETON 4
#define NUM_WRITEABLE_MODES 4
#define SETLIST 5
#define COPYSWAPSAVE 6
#define COPYSWAPSAVEWAIT 7
#define BANKSEARCH 8
#define SETLISTBANKSEARCH 9
#define SETLISTPRESETBUILD1 10
#define SETLISTPRESETBUILD2 11
#define SETLISTSAVE 12
#define SAVETYPE_NONE -1
#define SAVETYPE_LOOPS 0
#define SAVETYPE_MIDI 1
#define BUTTON_COUNT 10
#define ACTUAL_BUTTON_COUNT 6

int prevMode = 0;
int mode = 0;
int onePress = -1;
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

int modeColors[13][3] = {
    // LIVE red
    {255, 0, 0},
    // MIDI blue
    {0, 0, 255},
    // PRESET green
    {0, 255, 0},
    // LOOPER white
    {255, 255, 255},
    // ABLETON yellow
    {0, 255, 255},
    // SETLIST yellow
    {0, 255, 255},
    // COPYSWAPSAVE magenta
    {255, 50, 255},
    // COPYSWAPSAVEWAIT magenta
    {255, 50, 255},
    // BANKSEARCH green
    {0, 255, 0},
    // SETLISTBANKSEARCH green
    {0, 255, 0},
    // SETLISTPRESETBUILD1 green
    {0, 255, 0},
    // SETLISTPRESETBUILD2 green
    {0, 255, 0},
    // SETLISTSAVE green
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
    int setlistPreset = 0;

    // set state object
    int looper[3] = {0, 0, 0};
    state->setState(preset, setlistPreset, midi1, midi2, loops, looper);

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
            State* oldState = state->copy();

            buttons->updateStates(input);

            for (int i = 0; i < BUTTON_COUNT; i++) {
                press[i] = false;
                pressed[i] = false;
                pressHold[i] = false;
                release[i] = false;
            }
            buttons->detectEvents(press, pressed, pressHold, release);

        update();

            // If state has changed
            if (firstLoop || state->diff(oldState) || prevMode != mode) {
                firstLoop = false;
                render();

                storage->saveState(writableMode, state);
            }
        }
    }
    oldInput = input;
}

void nextMode() {
    prevMode = mode;
    mode++;
    if (mode > NUM_WRITEABLE_MODES) {
        mode = LIVE;
    }
    writableMode = mode;
}

void transition(int newMode) {
    prevMode = mode;
    if (newMode < COPYSWAPSAVEWAIT) {
        // writable modes are LIVE, MIDI, PRESET, LOOPER, ABLETON, and SETLIST
        writableMode = newMode;
    }
    mode = newMode;
}

void update() {
    // UPDATE STATE
    if (press[0]) {
        if (mode <= ABLETON)
        // cycle through modes
        nextMode();
        onePress = -1;
    }
    if (pressHold[0]) {
        if (mode == SETLIST) {
            transition(prevMode);
            onePress = -1;
        } else if (mode <= ABLETON) {
            transition(SETLIST);
            onePress = -1;
        }
    }

    if (mode == LIVE) {
        // press on 1-5 triggers true bypass loop toggle
        if (press[1]) {
            state->toggleLoop(0);
        }
        if (press[2]) {
            state->toggleLoop(1);
        }
        if (press[3]) {
            state->toggleLoop(2);
        }
        if (press[4]) {
            state->toggleLoop(3);
        }
        if (press[5]) {
            state->toggleLoop(4);
        }
        if (pressHold[5]) {
            saveType = SAVETYPE_LOOPS;
            saveLoops = state->getLoops();
            transition(COPYSWAPSAVE);
        }
    } else if (mode == MIDI) {
        // press on 1 & 2 trigger midi1 down/up
        if (press[1]) {
            state->midi1Down();
        }
        if (press[2]) {
            state->midi1Up();
        }
        if (press[3]) {
            state->midi2Down();
        }
        if (press[4]) {
            state->midi2Up();
        }
        if (press[5]) {
            state->toggleLoop(4);
        }
        if(pressHold[5]) {
            // transition to SAVE
            saveType = SAVETYPE_MIDI;
            saveMidi1 = state->getMidi1();
            saveMidi2 = state->getMidi2();
            transition(COPYSWAPSAVE);
        }
    } else if (mode == PRESET) {
        // press on 1 & 2 trigger bank down/up. trigger BANKSEARCH
        int presetNum;
        Preset* preset;
        if (press[1]) {
            state->bankDown();
            transition(BANKSEARCH);
        }
        if (press[2]) {
            state->bankUp();
            transition(BANKSEARCH);
        }
        if (press[3]) {
            presetNum = state->selectPatch(0);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
        }
        if (press[4]) {
            presetNum = state->selectPatch(1);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
        }
        if (press[5]) {
            presetNum = state->selectPatch(2);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
        }
        if (pressHold[3]) {
            srcPresetNum = state->getPresetNum(0);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold[4]) {
            srcPresetNum = state->getPresetNum(1);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold[5]) {
            srcPresetNum = state->getPresetNum(2);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
    } else if (mode == LOOPER) {
        // XXX should this go through state + render or be "special" in that we
        // can hard-code right here?
        //
        // press on 1, 2, 3 trigger relays
        if (press[1]) {
            state->activateLooperControl(0);
        }
        if (press[2]) {
            state->activateLooperControl(1);
        }
        if (press[3]) {
            state->activateLooperControl(2);
        }
        if (release[1]) {
            // set relay low
            state->deactivateLooperControl(0);
        }
        if (release[2]) {
            // set relay low
            state->deactivateLooperControl(1);
        }
        if (release[3]) {
            // set relay low
            state->deactivateLooperControl(2);
        }
    } else if (mode == ABLETON) {
        if (press[1]) {
            state->abletonPlay();
        }
        if (press[3]) {
            state->abletonUp();
        }
        if (press[5]) {
            state->abletonStop();
        }
    } else if (mode == SETLIST) {
        // press on 1 & 2 trigger bank down/up. trigger SETLISTBANKSEARCH
        int presetNum;
        int setlistPresetNum;
        Preset* preset;
        SetlistPreset* setlistPreset;
        if (press[1]) {
            state->setlistBankDown();
            transition(SETLISTBANKSEARCH);
            onePress = -1;
        }
        if (press[2]) {
            state->setlistBankUp();
            transition(SETLISTBANKSEARCH);
            onePress = -1;
        }
        if (press[3]) {
            // get setlist preset
            setlistPresetNum = state->selectSetlistPatch(0);
            setlistPreset = storage->getSetlistPresetByNum(setlistPresetNum);
            if (onePress = 3) {
                // play ableton loop
                state->abletonPlay(setlistPreset->getAbleton2());
            } else {
                onePress = 3;

                // get guitar preset
                presetNum = state->selectPatchByNum(setlistPreset->getPresetNum());
                preset = storage->getPresetByNum(presetNum);
                // activate guitar preset
                state->setLoops(preset->getLoops());
                state->setMidi1(preset->getMidi1());
                state->setMidi2(preset->getMidi2());
                // play ableton pad
                state->abletonPlay(setlistPreset->getAbleton1());
            }
        }
        if (press[4]) {
            // get setlist preset
            setlistPresetNum = state->selectSetlistPatch(1);
            setlistPreset = storage->getSetlistPresetByNum(setlistPresetNum);
            if (onePress = 4) {
                // play ableton loop
                state->abletonPlay(setlistPreset->getAbleton2());
            } else {
                onePress = 4;

                // get guitar preset
                presetNum = state->selectPatchByNum(setlistPreset->getPresetNum());
                preset = storage->getPresetByNum(presetNum);
                // activate guitar preset
                state->setLoops(preset->getLoops());
                state->setMidi1(preset->getMidi1());
                state->setMidi2(preset->getMidi2());
                // play ableton pad
                state->abletonPlay(setlistPreset->getAbleton1());
            }
        }
        if (press[5]) {
            // get setlist preset
            setlistPresetNum = state->selectSetlistPatch(2);
            setlistPreset = storage->getSetlistPresetByNum(setlistPresetNum);
            if (onePress = 5) {
                // play ableton loop
                state->abletonPlay(setlistPreset->getAbleton2());
            } else {
                onePress = 5;

                // get guitar preset
                presetNum = state->selectPatchByNum(setlistPreset->getPresetNum());
                preset = storage->getPresetByNum(presetNum);
                // activate guitar preset
                state->setLoops(preset->getLoops());
                state->setMidi1(preset->getMidi1());
                state->setMidi2(preset->getMidi2());
                // play ableton pad
                state->abletonPlay(setlistPreset->getAbleton1());
            }
        }
        if (pressHold[3]) {
            // enter SETLISTPRESETBUILD1 mode
        }
        // nothing for pressHold 4
        if (pressHold[5]) {
            // check state
            // if favorite is active
            //   switch back to guitar preset for active setlist preset
            // else
            //   activate favorite guitar preset
        }
    } else if (mode == COPYSWAPSAVE) {
        // when set, tempBank should be set to current bank
        if (press[1]) {
            state->bankDown();
        }
        if (press[2]) {
            state->bankUp();
        }
        if (pressHold[1] || pressHold[2]) {
            // state->selectPatchByNum(srcPresetNum);
            transition(prevMode);
        }
        if (press[3] || press[4] || press[5]) {
            if (press[3]) {
                targetPresetNum = state->getTempPresetNum(0);
            }
            if (press[4]) {
                targetPresetNum = state->getTempPresetNum(1);
            }
            if (press[5]) {
                targetPresetNum = state->getTempPresetNum(2);
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
                    state->selectPatchByNum(targetPresetNum);
                } else if (saveType == SAVETYPE_MIDI) {
                    storage->saveMidiToPreset(saveMidi1, saveMidi2, targetPresetNum);
                    state->selectPatchByNum(targetPresetNum);
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
            state->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            targetPresetNum = -1;
            transition(PRESET);
            copySwapSaveStart = -1;
        }

        if (pressHold[3] || pressHold[4] || pressHold[5]) {
            if (srcPreset) {
                storage->savePresetByNum(targetPreset, srcPresetNum);
            }
            state->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            transition(PRESET);
            copySwapSaveStart = -1;
        }
    } else if (mode == BANKSEARCH) {
        int presetNum;
        Preset* preset;
        if (press[1]) {
            state->bankDown();
        }
        if (press[2]) {
            state->bankUp();
        }
        if (pressHold[1] || pressHold[2]) {
            // in bank search, last preset from PRESET mode is still active
            // so just clear the temp bank and transition to preset
            state->clearTempBank();
            transition(PRESET);
        }
        if (press[3]) {
            presetNum = state->selectPatch(0, true);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
            transition(PRESET);
        }
        if (press[4]) {
            presetNum = state->selectPatch(1, true);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
            transition(PRESET);
        }
        if (press[5]) {
            presetNum = state->selectPatch(2, true);
            preset = storage->getPresetByNum(presetNum);
            state->setLoops(preset->getLoops());
            state->setMidi1(preset->getMidi1());
            state->setMidi2(preset->getMidi2());
            transition(PRESET);
        }
    } else if (mode == SETLISTBANKSEARCH) {
        if (press[1]) {
            state->setlistBankDown();
        }
        if (press[2]) {
            state->setlistBankUp();
        }
        if (pressHold[1] || pressHold[2]) {
            // in bank search, last preset from PRESET mode is still active
            // so just clear the temp bank and transition to preset
            state->clearSetlistTempBank();
            transition(SETLIST);
        }
        if (press[3]) {
            state->selectSetlistPatch(0, true);
            transition(SETLIST);
        }
        if (press[4]) {
            state->selectSetlistPatch(1, true);
            transition(SETLIST);
        }
        if (press[5]) {
            state->selectSetlistPatch(2, true);
            transition(SETLIST);
        }
    } else if (mode == SETLISTPRESETBUILD1) {
        // user chooses a guitar preset
    } else if (mode == SETLISTPRESETBUILD2) {
        // user chooses an ableton scene number
    } else if (mode == SETLISTSAVE) {
        // user chooses which bank + patch to save newly created preset on
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
    bool ledStates[9] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (mode == LIVE) {
        // light up which loops are active
        loops = state->loops;
        for (i = 0; i < 5; i++) {
            if((loops >> i) & 1) {
                ledStates[i] = true;
            }
        }
    } else if (mode == MIDI) {
        if (press[1] || pressed[1]) {
            ledStates[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledStates[1] = true;
        }
        if (press[3] || pressed[3]) {
            ledStates[2] = true;
        }
        if (press[4] || pressed[4]) {
            ledStates[3] = true;
        }
        // light up led5 if loop 5 is active
        loops = state->loops;
        if((loops >> 4) & 1) {
            ledStates[4] = true;
        }
    } else if (mode == PRESET) {
        if (press[1] || pressed[1]) {
            ledStates[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledStates[1] = true;
        }
        // light up bank leds
        int bank = state->getBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledStates[(3-i)+5] = true;
            }
        }
        ledStates[state->getPatch() + 2] = true;
    } else if (mode == BANKSEARCH) {
        if (press[1] || pressed[1]) {
            ledStates[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledStates[1] = true;
        }
        // light up bank leds
        int bank = state->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledStates[(3-i)+5] = true;
            }
        }
    } else if (mode == LOOPER) {
        if (press[1] || pressed[1]) {
            ledStates[0] = true;
        }
        if (press[2] || pressed[2]) {
            ledStates[1] = true;
        }
        if (press[3] || pressed[3]) {
            ledStates[2] = true;
        }
    } else if (mode == COPYSWAPSAVE) {
        // light up bank leds
        int bank = state->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledStates[(3-i)+5] = true;
            }
        }
    } else if (mode == COPYSWAPSAVEWAIT) {
        // light up bank leds
        int bank = state->getTempBank() + 1;
        for (i = 0; i < 4; i++) {
            if((bank >> i) & 1) {
                ledStates[(3-i)+5] = true;
            }
        }
        if (targetPresetNum != -1) {
            ledStates[state->getPatchForPresetNum(targetPresetNum) + 2] = true;
        }
    }
    lightgrid->setLeds(ledStates);

    lightgrid->writeShifter();

    // send midi program changes for midi1 and midi2
    int program = state->getMidi1();
    // Serial.println("SEND MIDI 1");
    // Serial.println(program);
    sendMidi(0xC1, program);

    program = state->getMidi2();
    // Serial.println("SEND MIDI 2");
    // Serial.println(program);
    sendMidi(0xC0, program);

    // trigger active loops, deactivate inactive loops

    // set looper control relays to high if activated
    // set looper control relays to low if deactivated
    // set all looper controls to neutral in the model
}
