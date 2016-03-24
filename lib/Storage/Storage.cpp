#include <Storage.h>

Storage::Storage() {
}

void Storage::saveState(int mode, State* state) {
    // first unsigned char is 5 bits of loops followed by a zero followed by 2 bits for mode
    unsigned char first = state->loops;

    // copy first two mode bits onto bits 6 & 7 of first byte
    if ((mode >> 0) & 1) {
        first |= 1 << 6;
    }
    if ((mode >> 1) & 1) {
        first |= 1 << 7;
    }

    EEPROM.write(0, first);
    EEPROM.write(1, state->midi1);
    EEPROM.write(2, state->midi2);
    EEPROM.write(3, state->currentPreset);
}

void Storage::savePresetByNum(Preset *preset, int num) {
    int base = 4;
    base += num;

    EEPROM.write(base, preset->getLoops());
    EEPROM.write(base + 1, preset->getMidi1());
    EEPROM.write(base + 2, preset->getMidi2());
}

void Storage::saveMidiToPreset(int midi1, int midi2, int num) {
    int base = 4;
    base += num;

    EEPROM.write(base + 1, midi1);
    EEPROM.write(base + 2, midi2);
}

void Storage::saveLoopsToPreset(unsigned char loops, int num) {
    int base = 4;
    base += num;

    EEPROM.write(base, loops);
}

Preset* Storage::getPresetByNum(int num) {
    int base = 4;
    base += num;

    unsigned char readLoops = EEPROM.read(base);
    unsigned char readMidi1 = EEPROM.read(base + 1);
    unsigned char readMidi2 = EEPROM.read(base + 2);

    Preset *preset = new Preset(readLoops, readMidi1, readMidi2);

    return preset;
}

SetlistPreset* Storage::getSetlistPresetByNum(int num) {
    int base = 4 + ( 3 * 15 );
    base += num;

    int readPreset   = EEPROM.read(base);
    int readAbleton1 = EEPROM.read(base + 1);
    int readAbleton2 = EEPROM.read(base + 2);

    SetlistPreset *setlistPreset = new SetlistPreset(readPreset, readAbleton1, readAbleton2);

    return setlistPreset;
}

unsigned char Storage::getStartupLoops() {
    unsigned char byte = EEPROM.read(0);

    // clear last two bits (mode)
    byte &= ~(1 << 7);
    byte &= ~(1 << 6);

    return byte;
}

int Storage::getStartupMode() {
    unsigned char byte = EEPROM.read(0);

    int mode = 0;
    // shift last 2 bits of byte 0 to get int 0-3
    int j = 0;
    for (int i = 6; i < 8; i++) {
        mode |= ((byte >> i) & 1) << j;
        j++;
    }

    return mode;
}

int Storage::getStartupMidi1() {
    // byte 1
    return EEPROM.read(1);
}

int Storage::getStartupMidi2() {
    // byte 2
    return EEPROM.read(2);
}

int Storage::getStartupPreset() {
    // byte 3
    return EEPROM.read(3);
}
