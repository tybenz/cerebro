#ifndef Storage_h
#define Storage_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Preset.h>
#include <SetlistPreset.h>
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
        SetlistPreset* getSetlistPresetByNum(int num);
        unsigned char getStartupLoops();
        int getStartupMode();
        int getStartupMidi1();
        int getStartupMidi2();
        int getStartupPreset();
        void savePresetNumToSetlistPreset(int presetNum, int num);
        void saveAbleton1ToSetlistPreset(int ableton1, int num);
        void saveAbleton2ToSetlistPreset(int ableton2, int num);
};

#endif //Storage_h
