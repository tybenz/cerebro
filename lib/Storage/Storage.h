#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <EEPROM.h>

class Storage
{
    public:
        Storage();
        void saveState(int mode, unsigned char loops, unsigned char midi1, unsigned char midi2, int lastPreset);
        void savePresetByBankAndPatch(int bank, int patch, unsigned char loops, unsigned char midi1, unsigned char midi2);
        void savePresetByNum(int num, unsigned char loops, unsigned char midi1, unsigned char midi2);
};

#endif //Storage_h
