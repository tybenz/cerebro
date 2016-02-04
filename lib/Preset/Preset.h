#ifndef Preset_h
#define Preset_h

#include <Arduino.h>

class Preset
{
    private:
        unsigned char loops;
        int midi1;
        int midi2;
    public:
        Preset(unsigned char loops, int midi1, int midi2);
        unsigned char getLoops();
        int getMidi1();
        int getMidi2();
};

#endif //Preset_h
