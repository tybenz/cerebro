#include <Storage.h>

Storage::Storage() {
}

void Storage::saveState(int mode, State* state) {
    // first unsigned char is 5 bits of loops followed by a zero followed by 2 bits for mode
    unsigned char first = state->loops;
    first << 0;
    first << 1;
    first << 1;
    first = first & mode;

    unsigned char stateToWrite[4] = {
      first, state->midi1, state->midi2, state->currentPreset
    };

    for (int i = 0; i < 4; i++) {
        EEPROM.write(i, stateToWrite[i]);
    }
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
