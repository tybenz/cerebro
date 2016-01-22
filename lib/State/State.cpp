#include <State.h>

int PPB = 3; // patches per bank
int NUM_BANKS = 15; // binary 4 digits (not zero)
int MAX_PRESETS = PPB * NUM_BANKS;

State::State() : mode(0), currentPreset(0), tempBank(-1), midi1(0x00), midi2(0x00), loops(0x08) {
}

void State::setState(int newMode, int newPreset, int newMidi1, int newMidi2, unsigned char newLoops) {
    mode = newMode;
    currentPreset = newPreset;
    midi1 = newMidi1;
    midi2 = newMidi2;
    loops = newLoops;
}

void State::midi1Up() {
    midi1++;
    if (midi1 > 127) {
        midi1 = 127;
    }
}

void State::midi1Down() {
    midi1--;
    if (midi1 < 0) {
        midi1 = 0;
    }
}

void State::midi2Up() {
    midi2++;
    if (midi2 > 127) {
        midi2 = 127;
    }
}

void State::midi2Down() {
    midi2--;
    if (midi2 < 0) {
        midi2 = 0;
    }
}

void State::nextMode() {
    mode++;
    if (mode > 3) {
        mode = 0;
    }
}

void State::bankUp() {
    tempBank = getBank() + 1;
    if (tempBank > NUM_BANKS - 1) {
        tempBank = NUM_BANKS - 1;
    }
}

void State::bankDown() {
    tempBank = getBank() - 1;
    if (tempBank < 0) {
        tempBank = 0;
    }
}

void State::selectPatch(int num) {
    currentPreset = getBank() * PPB + num;
}

void State::activateLoop(int num) {
    loops |= 1 << num;
}

void State::deactivateLoop(int num) {
    loops &= ~(1 << num);
}

void State::toggleLoop(int num) {
    loops ^= 1 << num;
}

int State::getBank() {
    /* return currentPreset / PPB; */
    return currentPreset / PPB;
}

int State::getPatch() {
    /* return currentPreset % PPB; */
    return currentPreset % PPB;
}
