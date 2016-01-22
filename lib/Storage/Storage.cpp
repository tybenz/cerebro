#include <Storage.h>

Storage::Storage() {
  unsigned char value = EEPROM.read(0);
}

void Storage::saveState(int mode, unsigned char loops, unsigned char midi1, unsigned char midi2, int lastPreset) {
    // first unsigned char is 5 bits of loops followed by a zero followed by 2 bits for mode
    unsigned char first = mode;
    first << 0;
    first << 1;
    first << 1;
    first = first & mode;

    unsigned char state[4] = {
      first, midi1, midi2, lastPreset
    };

    for (int i = 0; i < 4; i++) {
        EEPROM.write(i, state[i]);
    }
}

void Storage::savePresetByBankAndPatch(int bank, int patch, unsigned char loops, unsigned char midi1, unsigned char midi2) {
    int num = (bank * 3) + (patch - 1);
    savePresetByNum(num, loops, midi1, midi2);
}

void Storage::savePresetByNum(int num, unsigned char loops, unsigned char midi1, unsigned char midi2) {
    int base = 4;
    base += num;

    EEPROM.write(base, loops);
    EEPROM.write(base + 1, midi1);
    EEPROM.write(base + 2, midi2);
}
