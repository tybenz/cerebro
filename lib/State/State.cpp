#include <State.h>

int PPB = 3; // patches per bank
int NUM_BANKS = 15; // binary 4 digits (not zero)
int MAX_PRESETS = PPB * NUM_BANKS;

State::State() : currentPreset(0), tempBank(-1), midi1(0x00), midi2(0x00), loops(0x00) {
    for (int i = 0; i < 3; i++) {
        looper[i] = 0;
    }
}

void State::setState(int newPreset, int newMidi1, int newMidi2, unsigned char newLoops, int newLooper[]) {
    currentPreset = newPreset;
    midi1 = newMidi1;
    midi2 = newMidi2;
    loops = newLoops;
    for (int i = 0; i < 3; i++) {
        looper[i] = newLooper[i];
    }
}

boolean State::diff(State* state) {
    if (currentPreset != state->currentPreset) {
        return true;
    }
    if (midi1 != state->midi1) {
        return true;
    }
    if (midi2 != state->midi2) {
        return true;
    }
    if (loops != state->loops) {
        return true;
    }
    return false;
}

State* State::copy() {
    State* newState;
    newState->setState(currentPreset, midi1, midi2, loops, looper);
    return newState;
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

void State::clearTempBank() {
    tempBank = -1;
}

void State::selectPatchByNum(int num) {
    currentPreset = num;
}

void State::selectPatch(int num) {
    currentPreset = getBank() * PPB + num;
}

void State::selectPatch(int num, boolean useTempBank) {
    currentPreset = tempBank * PPB + num;
    clearTempBank();
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

int State::getPresetNum(int num) {
    return getBank() * PPB + num;
}

unsigned char State::getLoops() {
    return loops;
}

int State::getMidi1() {
    return midi1;
}

int State::getMidi2() {
    return midi2;
}

void State::activateLooperControl(int num) {
    looper[num] = 1;
}

void State::deactivateLooperControl(int num) {
    looper[num] = 0;
}

void State::neutralizeLooperControl(int num) {
    looper[num] = -1;
}
