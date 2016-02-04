#include <LightGrid.h>
#include <Storage.h>
#include <State.h>
#include <Buttons.h>
#include <Preset.h>

LightGrid* lightgrid;
Storage* storage = new Storage();
State* state = new State();
Buttons* buttons = new Buttons();

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

int modeColors[7][3] = {
    // LIVE red
    {255, 0, 0},
    // MIDI blue
    {0, 0, 255},
    // PRESET green
    {0, 255, 0},
    // LOOPER yellow
    {255, 255, 0},
    // COPYSWAPSAVE magenta
    {255, 50, 255},
    // COPYSWAPSAVEWAIT magenta
    {255, 50, 255},
    // BANKSEARCH green
    {0, 255, 0}
};

int writableMode = 0;

void setup() {
    lightgrid = new LightGrid(3, 4, 5, 6);
    Serial.begin(9600);
    Serial.println(10000);
    pinMode(A5, INPUT_PULLUP); // sets analog pin for input
    // read state from EEPROM
    // set state object and render
}

int oldInput = 0;

void loop() {
    int input = analogRead(A5); // get the analog value
    if (input != oldInput) {
        delay(50);
        int input2 = analogRead(A5);
        if (input - input2 < 20 && input - input2 > -20) {
            oldInput = input;
            int i = 0;
            State* oldState = state->copy();

            buttons->updateStates(input);

            bool presses[BUTTON_COUNT] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            bool presseds[BUTTON_COUNT] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            bool pressHolds[BUTTON_COUNT] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            bool releases[BUTTON_COUNT] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            buttons->detectEvents(presses, presseds, pressHolds, releases);

            // save bools into readable vars
            bool pressMode     = presses[0];
            bool press1        = presses[1];
            bool press2        = presses[2];
            bool press3        = presses[3];
            bool press4        = presses[4];
            bool press5        = presses[5];
            bool press6        = presses[6];
            bool press7        = presses[7];
            bool press8        = presses[8];
            bool press9        = presses[9];
            bool pressedMode     = presseds[0];
            bool pressed1        = presseds[1];
            bool pressed2        = presseds[2];
            bool pressed3        = presseds[3];
            bool pressed4        = presseds[4];
            bool pressed5        = presseds[5];
            bool pressed6        = presseds[6];
            bool pressed7        = presseds[7];
            bool pressed8        = presseds[8];
            bool pressed9        = presseds[9];
            bool pressHoldMode = pressHolds[0];
            bool pressHold1    = pressHolds[1];
            bool pressHold2    = pressHolds[2];
            bool pressHold3    = pressHolds[3];
            bool pressHold4    = pressHolds[4];
            bool pressHold5    = pressHolds[5];
            bool pressHold6    = pressHolds[6];
            bool pressHold7    = pressHolds[7];
            bool pressHold8    = pressHolds[8];
            bool pressHold9    = pressHolds[9];
            bool releaseMode   = releases[0];
            bool release1      = releases[1];
            bool release2      = releases[2];
            bool release3      = releases[3];
            bool release4      = releases[4];
            bool release5      = releases[5];
            bool release6      = releases[6];
            bool release7      = releases[7];
            bool release8      = releases[8];
            bool release9      = releases[9];

            // UPDATE STATE
            if (pressMode) {
                // cycle through modes
                nextMode();
            }

            if (mode == LIVE) {
                // press on 1-5 triggers true bypass loop toggle
                if (press1) {
                    state->toggleLoop(0);
                }
                if (press2) {
                    state->toggleLoop(1);
                }
                if (press3) {
                    state->toggleLoop(2);
                }
                if (press4) {
                    state->toggleLoop(3);
                }
                if (press5) {
                    state->toggleLoop(4);
                }
                if (pressHold5) {
                    saveType = SAVETYPE_LOOPS;
                    saveLoops = state->getLoops();
                    transition(COPYSWAPSAVE);
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
                    state->toggleLoop(4);
                }
                if(pressHold5) {
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
                    srcPresetNum = state->getPresetNum(1);
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
                    // state->selectPatchByNum(srcPresetNum);
                    transition(prevMode);
                }
                if (press3 || press4 || press5) {
                    if (press3) {
                        targetPresetNum = state->getTempPresetNum(0);
                    }
                    if (press4) {
                        targetPresetNum = state->getTempPresetNum(1);
                    }
                    if (press5) {
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
                        transition(PRESET);
                    }
                }
            } else if (mode == COPYSWAPSAVEWAIT) {
                // We wait to see if there is a press hold, which forces a swap
                if (copySwapSaveStart == -1) {
                    copySwapSaveStart = millis();
                } else if (millis() - copySwapSaveStart > 2000) {
                    state->selectPatchByNum(targetPresetNum);
                    delete targetPreset;
                    transition(PRESET);
                    copySwapSaveStart = -1;
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
                if (press1) {
                    state->bankDown();
                }
                if (press2) {
                    state->bankUp();
                }
                if (pressHold1 || pressHold2) {
                    // in bank search, last preset from PRESET mode is still active
                    // so just clear the temp bank and transition to preset
                    state->clearTempBank();
                    transition(PRESET);
                }
                if (press3) {
                    state->selectPatch(0, true);
                    transition(PRESET);
                }
                if (press4) {
                    state->selectPatch(1, true);
                    transition(PRESET);
                }
                if (press5) {
                    state->selectPatch(2, true);
                    transition(PRESET);
                }
            }

            // If state has changed
            if (state->diff(oldState)) {
                // RENDER
                // light up leds according to state & mode
                int* colors = modeColors[mode];
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
                    if (press1 || pressed1) {
                        ledStates[0] = true;
                    }
                    if (press2 || pressed2) {
                        ledStates[1] = true;
                    }
                    if (press3 || pressed3) {
                        ledStates[2] = true;
                    }
                    if (press4 || pressed4) {
                        ledStates[3] = true;
                    }
                    // light up led5 if loop 5 is active
                    loops = state->loops;
                    if((loops >> 4) & 1) {
                        ledStates[4] = true;
                    }
                } else if (mode == PRESET) {
                    if (press1 || pressed1) {
                        ledStates[0] = true;
                    }
                    if (press2 || pressed2) {
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
                    if (press1 || pressed1) {
                        ledStates[0] = true;
                    }
                    if (press2 || pressed2) {
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
                    if (press1 || pressed1) {
                        ledStates[0] = true;
                    }
                    if (press2 || pressed2) {
                        ledStates[1] = true;
                    }
                    if (press3 || pressed3) {
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
                }
                lightgrid->setLeds(ledStates);

                // send midi program changes for midi1 and midi2
                // trigger active loops, deactivate inactive loops

                // set looper control relays to high if activated
                // set looper control relays to low if deactivated
                // set all looper controls to neutral in the model

                // WRITE to storage
                storage->saveState(writableMode, state);
            }
        }
        lightgrid->writeShifter();
    }
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
    if (newMode < 4) {
        writableMode = newMode;
    }
    mode = newMode;
}
