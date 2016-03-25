#include <SetlistPreset.h>

SetlistPreset::SetlistPreset(int initPresetNum, int initAbleton1, int initAbleton2) {
    presetNum = initPresetNum;
    ableton1 = initAbleton1;
    ableton2 = initAbleton2;
}

int SetlistPreset::getPresetNum() {
    return presetNum;
}

int SetlistPreset::getAbleton1() {
    return ableton1;
}

int SetlistPreset::getAbleton2() {
    return ableton2;
}

void SetlistPreset::setPresetNum(int num) {
    presetNum = num;
}

void SetlistPreset::setAbleton1(int num) {
    ableton1 = num;
}

void SetlistPreset::setAbleton2(int num) {
    ableton2 = num;
}
