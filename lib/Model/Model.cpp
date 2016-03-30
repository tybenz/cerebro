#include <Model.h>

int PPB = 3; // patches per bank
int NUM_BANKS = 15; // binary 3 digits (not zero)
int MAX_PRESETS = PPB * NUM_BANKS;

Model::Model() : currentPreset(0), currentSetlistPreset(0), tempBank(-1), setlistTempBank(-1), currentAbleton(1), midi1(0x00), midi2(0x00), loops(0x00) {
    for (int i = 0; i < 3; i++) {
        looper[i] = 0;
    }
}

void Model::setModel(int newPreset, int newSetlistPreset, int newMidi1, int newMidi2, int newAbleton, unsigned char newLoops, int newLooper[]) {
    currentPreset = newPreset;
    currentSetlistPreset = newSetlistPreset;
    midi1 = newMidi1;
    midi2 = newMidi2;
    currentAbleton = newAbleton;
    loops = newLoops;
    for (int i = 0; i < 3; i++) {
        looper[i] = newLooper[i];
    }
}

bool Model::diff(Model* model) {
    if (currentPreset != model->currentPreset) {
        return true;
    }
    if (currentSetlistPreset != model->currentSetlistPreset) {
        return true;
    }
    if (midi1 != model->midi1) {
        return true;
    }
    if (midi2 != model->midi2) {
        return true;
    }
    if (loops != model->loops) {
        return true;
    }
    return false;
}

Model* Model::copy() {
    Model* newModel;
    newModel->setModel(currentPreset, currentSetlistPreset, midi1, midi2, currentAbleton, loops, looper);
    return newModel;
}

void Model::midi1Up() {
    midi1++;
    if (midi1 > 127) {
        midi1 = 127;
    }
    Serial.println("MIDI1");
    Serial.println(midi1);
}

void Model::midi1Down() {
    midi1--;
    if (midi1 < 0) {
        midi1 = 0;
    }
    Serial.println("MIDI1");
    Serial.println(midi1);
}

void Model::midi2Up() {
    midi2++;
    if (midi2 > 127) {
        midi2 = 127;
    }
    Serial.println("MIDI2");
    Serial.println(midi2);
}

void Model::midi2Down() {
    midi2--;
    if (midi2 < 0) {
        midi2 = 0;
    }
    Serial.println("MIDI2");
    Serial.println(midi2);
}

void Model::bankUp() {
    if (tempBank == -1) {
        tempBank = getBank() + 1;
    } else {
        tempBank++;
    }
    if (tempBank > NUM_BANKS - 1) {
        tempBank = 0;
    }
}

void Model::bankDown() {
    if (tempBank == -1) {
        tempBank = getBank() - 1;
    } else {
        tempBank--;
    }
    if (tempBank < 0) {
        tempBank = NUM_BANKS - 1;
    }
}

void Model::clearTempBank() {
    tempBank = -1;
}

void Model::setlistBankUp() {
    if (setlistTempBank == -1) {
        setlistTempBank = getSetlistBank() + 1;
    } else {
        setlistTempBank++;
    }
    if (setlistTempBank > NUM_BANKS - 1) {
        setlistTempBank = 0;
    }
}

void Model::setlistBankDown() {
    if (setlistTempBank == -1) {
        setlistTempBank = getSetlistBank() - 1;
    } else {
        setlistTempBank--;
    }
    if (setlistTempBank < 0) {
        setlistTempBank = NUM_BANKS - 1;
    }
}

void Model::clearSetlistTempBank() {
    setlistTempBank = -1;
}

int Model::selectPatchByNum(int num) {
    currentPreset = num;
}

int Model::selectPatch(int num) {
    selectPatchByNum(getBank() * PPB + num);
}

int Model::selectPatch(int num, bool useTempBank) {
    selectPatchByNum(setlistTempBank * PPB + num);
    clearTempBank();
}

int Model::selectSetlistPatchByNum(int num) {
    currentSetlistPreset = num;
}

int Model::selectSetlistPatch(int num) {
    selectSetlistPatchByNum(getSetlistBank() * PPB + num);
}

int Model::selectSetlistPatch(int num, bool useTempBank) {
    selectSetlistPatchByNum(setlistTempBank * PPB + num);
    clearSetlistTempBank();
}

void Model::activateLoop(int num) {
    loops |= 1 << num;
}

void Model::deactivateLoop(int num) {
    loops &= ~(1 << num);
}

void Model::toggleLoop(int num) {
    loops ^= 1 << num;
}

void Model::setLoops(unsigned char newLoops) {
    loops = newLoops;
}

int Model::getBank() {
    /* return currentPreset / PPB; */
    return currentPreset / PPB;
}

int Model::getSetlistBank() {
    /* return currentPreset / PPB; */
    return currentSetlistPreset / PPB;
}

int Model::getTempBank() {
    if (tempBank == -1) {
        return getBank();
    } else {
        return tempBank;
    }
}

int Model::getSetlistTempBank() {
    if (setlistTempBank == -1) {
        return getSetlistBank();
    } else {
        return setlistTempBank;
    }
}

int Model::getPatch() {
    /* return currentPreset % PPB; */
    return currentPreset % PPB;
}

int Model::getSetlistPatch() {
    return currentSetlistPreset % PPB;
}

int Model::getPatchForPresetNum(int num) {
    return num % PPB;
}

int Model::getPresetNum(int num) {
    return getBank() * PPB + num;
}

int Model::getTempPresetNum(int num) {
    return getTempBank() * PPB + num;
}

int Model::getTempSetlistPresetNum(int num) {
    return getSetlistTempBank() * PPB + num;
}

unsigned char Model::getLoops() {
    return loops;
}

int Model::getMidi1() {
    return midi1;
}

int Model::getMidi2() {
    return midi2;
}

void Model::activateLooperControl(int num) {
    looper[num] = 1;
}

void Model::deactivateLooperControl(int num) {
    looper[num] = 0;
}

void Model::neutralizeLooperControl(int num) {
    looper[num] = -1;
}

void Model::setMidi1(int num) {
    midi1 = num;
}

void Model::setMidi2(int num) {
    midi2 = num;
}

void Model::abletonPlay() {
}

void Model::abletonUp() {
}

void Model::abletonStop() {
}

void Model::abletonPlay(int num) {
}

void Model::abletonSelectUp() {
    currentAbleton++;
    if (currentAbleton > 15) {
        currentAbleton = 15;
    }
}

void Model::abletonSelectDown() {
    if (currentAbleton < 1) {
    currentAbleton--;
        currentAbleton = 1;
    }
}
