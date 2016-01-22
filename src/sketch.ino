#include <LightGrid.h>
#include <Storage.h>
#include <State.h>
#include <Buttons.h>
#include <Preset.h>

LightGrid* lightgrid = new LightGrid(10, 11, 12);
Storage* storage = new Storage();
State* state = new State();
Buttons* buttons = new Buttons();

#define LIVE 0
#define MIDI 1
#define PRESET 2
#define LOOPER 3
#define COPYSWAPSAVE 4
#define COPYSWAPSAVEWAIT 5
#define BANKSEARCH 6
#define SAVETYPE_LOOPS 0
#define SAVETYPE_MIDI 1

int prevMode = 0;
int mode = 0;

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


void setup() {
    Serial.begin(31250);
    // read state from EEPROM
    // set state object and render
}

void loop() {
    State* oldState = state->copy();

    // CHECK BUTTONS
    unsigned char newStates = 0x00;
    int mode    = digitalRead(modeButtonPin);
    newStates     |= mode << 5;
    int button1 = digitalRead(button1Pin);
    newStates     |= mode << 4;
    int button2 = digitalRead(button2Pin);
    newStates     |= mode << 3;
    int button3 = digitalRead(button3Pin);
    newStates     |= mode << 2;
    int button4 = digitalRead(button4Pin);
    newStates     |= mode << 1;
    int button5 = digitalRead(button5Pin);
    newStates     |= mode << 0;
    buttons->updateStates(newStates);

    boolean** events = buttons->detectEvents();
    boolean* presses = events[0];
    boolean* pressHolds = events[1];
    boolean* releases = events[1];

    // save bools into readable vars
    boolean pressMode     = presses[0];
    boolean press1        = presses[1];
    boolean press2        = presses[2];
    boolean press3        = presses[3];
    boolean press4        = presses[4];
    boolean press5        = presses[5];
    boolean press6        = presses[6];
    boolean press7        = presses[7];
    boolean press8        = presses[8];
    boolean press9        = presses[9];
    boolean pressHoldMode = pressHolds[0];
    boolean pressHold1    = pressHolds[1];
    boolean pressHold2    = pressHolds[2];
    boolean pressHold3    = pressHolds[3];
    boolean pressHold4    = pressHolds[4];
    boolean pressHold5    = pressHolds[5];
    boolean pressHold6    = pressHolds[6];
    boolean pressHold7    = pressHolds[7];
    boolean pressHold8    = pressHolds[8];
    boolean pressHold9    = pressHolds[9];
    boolean releaseMode   = releases[0];
    boolean release1      = releases[1];
    boolean release2      = releases[2];
    boolean release3      = releases[3];
    boolean release4      = releases[4];
    boolean release5      = releases[5];
    boolean release6      = releases[6];
    boolean release7      = releases[7];
    boolean release8      = releases[8];
    boolean release9      = releases[9];

    // UPDATE STATE
    if (pressMode) {
        // cycle through modes
        nextMode();
    }

    if (mode == LIVE) {
        // press on 1-5 triggers true bypass loop toggle
        if (press1) {
            state->toggleLoop(1);
        }
        if (press2) {
            state->toggleLoop(2);
        }
        if (press3) {
            state->toggleLoop(3);
        }
        if (press4) {
            state->toggleLoop(4);
        }
        if (press5) {
            state->toggleLoop(5);
        }
        if (pressHold9) {
            saveType = SAVETYPE_LOOPS;
            unsigned char saveLoops = state->getLoops();
            mode = COPYSWAPSAVE;
        }
    } else if (mode == MIDI) {
        // press on 1 & 2 trigger midi1 down/up
        if (press1) {
            state->midi1Down();
        }
        if (press2) {
            state->midi1Up();
        }
        if (press3) {
            state->midi2Down();
        }
        if (press4) {
            state->midi2Up();
        }
        if (press5) {
            state->toggleLoop(5);
        }
        if(pressHold9) {
            // transition to SAVE
            saveType = SAVETYPE_MIDI;
            saveMidi1 = state->getMidi1();
            saveMidi2 = state->getMidi2();
            transition(COPYSWAPSAVE);
        }
    } else if (mode == PRESET) {
        // press on 1 & 2 trigger bank down/up. trigger BANKSEARCH
        if (press1) {
            state->bankDown();
            transition(BANKSEARCH);
        }
        if (press2) {
            state->bankUp();
            transition(BANKSEARCH);
        }
        if (press3) {
            state->selectPatch(0);
        }
        if (press4) {
            state->selectPatch(1);
        }
        if (press5) {
            state->selectPatch(2);
        }
        if (pressHold3) {
            srcPresetNum = state->getPresetNum(0);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold4) {
            srcPresetNum = state->getPresetNum(0);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
        if (pressHold5) {
            srcPresetNum = state->getPresetNum(2);
            srcPreset = storage->getPresetByNum(srcPresetNum);
            transition(COPYSWAPSAVE);
        }
    } else if (mode == LOOPER) {
        // XXX should this go through state + render or be "special" in that we
        // can hard-code right here?
        //
        // press on 1, 2, 3 trigger relays
        if (press1) {
            state->activateLooperControl(0);
        }
        if (press2) {
            state->activateLooperControl(1);
        }
        if (press3) {
            state->activateLooperControl(2);
        }
        if (release1) {
            // set relay low
            state->deactivateLooperControl(0);
        }
        if (release2) {
            // set relay low
            state->deactivateLooperControl(1);
        }
        if (release3) {
            // set relay low
            state->deactivateLooperControl(2);
        }
    } else if (mode == COPYSWAPSAVE) {
        // when set, tempBank should be set to current bank
        if (press1) {
            state->bankDown();
        }
        if (press2) {
            state->bankUp();
        }
        if (pressHold1 || pressHold2) {
            state->selectPatchByNum(srcPresetNum);
            transition(PRESET);
        }
        if (press1 || press2 || press3) {
            if (press1) {
                targetPresetNum = state->getPresetNum(0);
            }
            if (press2) {
                targetPresetNum = state->getPresetNum(1);
            }
            if (press3) {
                targetPresetNum = state->getPresetNum(2);
            }
            targetPreset = storage->getPresetByNum(targetPresetNum);

            if (prevMode == PRESET) {
                storage->savePresetByNum(srcPreset, targetPresetNum);
                delete srcPreset;
            } else if (saveType) {
                if (saveType == SAVETYPE_LOOPS) {
                    storage->saveLoopsToPreset(saveLoops, targetPresetNum);
                } else if (saveType == SAVETYPE_MIDI) {
                    storage->saveMidiToPreset(saveMidi1, saveMidi2, targetPresetNum);
                }
            }
            transition(COPYSWAPSAVEWAIT);
        }
    } else if (mode == COPYSWAPSAVEWAIT) {
        if (copySwapSaveStart == -1) {
            copySwapSaveStart = millis();
        } else if (millis() - copySwapSaveStart > 2000) {
            state->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            transition(PRESET);
        }

        if (pressHold3 || pressHold4 || pressHold5) {
            if (srcPreset) {
                storage->savePresetByNum(targetPreset, srcPresetNum);
            }
            state->selectPatchByNum(targetPresetNum);
            delete targetPreset;
            transition(PRESET);
        }
    } else if (mode == BANKSEARCH) {
        if (pressHold1 || pressHold2) {
            // in bank search, last preset from PRESET mode is still active
            // so just clear the temp bank and transition to preset
            state->clearTempBank();
            transition(PRESET);
        }
        if (press3) {
            state->selectPatch(0, true);
        }
        if (press4) {
            state->selectPatch(1, true);
        }
        if (press5) {
            state->selectPatch(2, true);
        }
    }

    boolean hasChanged = state->diff(oldState);
    // RENDER
    // light up leds according to state & mode
    // send midi program changes if there's been a change (PRESET or MIDI)
    // trigger appropriate loops if there's been a change (PRESET or LIVE)
    // set looper control relays to high if activated
    // set looper control relays to low if deactivated
    // set all looper controls to neutral in the model


    // write to storage if anything has changed
}

void nextMode() {
    prevMode = mode;
    mode++;
    if (mode > 3) {
        mode = LIVE;
    }
}

void transition(int newMode) {
    prevMode = mode;
    mode = newMode;
}
