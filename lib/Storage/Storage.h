#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Preset.h>
#include <State.h>

class Storage
{
    public:
        Storage();
        void saveState(int mode, State* state);
        void savePresetByNum(Preset *preset, int num);
        void saveLoopsToPreset(unsigned char loops, int num);
        void saveMidiToPreset(int midi1, int midi2, int num);
        Preset* getPresetByNum(int num);
};

#endif //Storage_h
