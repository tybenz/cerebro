#ifndef SetlistPreset_h
#define SetlistPreset_h

#include <Arduino.h>

class SetlistPreset
{
    private:
        int presetNum;
        int ableton1;
        int ableton2;
    public:
        SetlistPreset(int presetNum, int ableton1, int ableton2);
        int getPresetNum();
        int getAbleton1();
        int getAbleton2();
        void setPresetNum(int num);
        void setAbleton1(int num);
        void setAbleton2(int num);
};

#endif //SetlistPreset_h
