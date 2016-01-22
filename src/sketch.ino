#include <LightGrid.h>
#include <Storage.h>
#include <State.h>
#include <Buttons.h>

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

int mode = 0;

int modeButtonPin = 0;
int button1Pin    = 1;
int button2Pin    = 2;
int button3Pin    = 3;
int button4Pin    = 4;
int button5Pin    = 5;

long copySwapSaveStart = -1;

void setup() {
    Serial.begin(31250);
    // read state from EEPROM
    // set state object and render
}

void loop() {
    // CHECK BUTTONS
    unsigned char states = 0x00;
    int mode    = digitalRead(modeButtonPin);
    states     |= mode << 5;
    int button1 = digitalRead(button1Pin);
    states     |= mode << 4;
    int button2 = digitalRead(button2Pin);
    states     |= mode << 3;
    int button3 = digitalRead(button3Pin);
    states     |= mode << 2;
    int button4 = digitalRead(button4Pin);
    states     |= mode << 1;
    int button5 = digitalRead(button5Pin);
    states     |= mode << 0;
    buttons->updateStates(states);


    boolean** events = buttons->detectEvents();
    boolean* presses = events[0];
    boolean* pressHolds = events[1];

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
            // transition to SAVE
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
        }
    } else if (mode == PRESET) {
        // press on 1 & 2 trigger bank down/up. trigger BANKSEARCH
        if (press1) {
            state->bankDown();
        }
        if (press2) {
            state->bankUp();
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
            // copyPreset = State->getPreset(0);
            // transition COPYSWAPSAVE
        }
        if (pressHold4) {
            // copyPreset = State->getPreset(1);
            // transition COPYSWAPSAVE
        }
        if (pressHold5) {
            // copyPreset = State->getPreset(2);
            // transition COPYSWAPSAVE
        }
    } else if (mode == LOOPER) {
        // XXX should this go through state + render or be "special" in that we
        // can hard-code right here?
        //
        // press on 1, 2, 3 trigger relays
    } else if (mode == COPYSWAPSAVE) {
        // when set, tempBank should be set to current bank
        if (press1) {
            state->bankDown();
        }
        if (press2) {
            state->bankUp();
        }
        if (pressHold1 || pressHold2) {
            // trigger PRESET with lastPreset
        }
        if (press3) {
            // if (copyPreset) {
            //     state->save(copyPreset, 0);
            // } else if (saveType) {
            //     if (saveType == LOOPS) {
            //         state->saveLoopsToPreset(saveLoops, 0);
            //     } else if (saveType == MIDI) {
            //         state->saveLoopsToPreset(saveMidi, 0);
            //     }
            // }
            // save overwriteTarget
            // set prevCopy
            // transition COPYSWAPSAVEWAIT
        }
    } else if (mode == COPYSWAPSAVEWAIT) {
        if (copySwapSaveStart == -1) {
            copySwapSaveStart = millis();
        } else if (millis() - copySwapSaveStart > 2000) {
            // activate newPreset
            // transition to PRESET
            // return
        }

        if (pressHold3) {
            // if prevCopy -> undo and do swap
            // activate newPreset
            // transition to PRESET
        }
        if (pressHold4) {
            // if prevCopy -> undo and do swap
            // activate newPreset
            // transition to PRESET
        }
        if (pressHold5) {
            // if prevCopy -> undo and do swap
            // activate newPreset
            // transition to PRESET
        }
    } else if (mode == BANKSEARCH) {
        if (pressHold1 || pressHold2) {
            // activate lastPreset
            // transition to PRESET
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

    // re-render
    // write to storage if anything has changed
}

void nextMode() {
    mode++;
    if (mode > 3) {
        mode = LIVE;
    }
}
