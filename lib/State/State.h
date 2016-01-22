#ifndef State_h
#define State_h

#include <Arduino.h>
#include <Shifter.h>

class State
{
    private:
        int mode;
        int currentPreset;
        int tempBank;
        int midi1;
        int midi2;
        unsigned char loops;
        int getBank();
        int getPatch();
    public:
        State();
        void setState(int newMode, int newPreset, int newMidi1, int newMidi2, unsigned char newLoops);
        void midi1Up();
        void midi1Down();
        void midi2Up();
        void midi2Down();
        void nextMode();
        void activateLoop(int num);
        void deactivateLoop(int num);
        void toggleLoop(int num);
};

#endif //State_h
