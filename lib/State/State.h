#ifndef State_h
#define State_h

#include <Arduino.h>
#include <Shifter.h>

class State
{
    public:
        int currentPreset;
        int tempBank;
        int midi1;
        int midi2;
        int looper[3];
        unsigned char loops;
        int getBank();
        int getPatch();
        State();
        void setState(int newPreset, int newMidi1, int newMidi2, unsigned char newLoops, int newLooper[]);
        bool diff(State* state);
        State* copy();
        void midi1Up();
        void midi1Down();
        void midi2Up();
        void midi2Down();
        void bankUp();
        void bankDown();
        void clearTempBank();
        void selectPatchByNum(int num);
        void selectPatch(int num);
        void selectPatch(int num, bool useTempBank);
        void activateLoop(int num);
        void deactivateLoop(int num);
        void toggleLoop(int num);
        int getPresetNum(int num);
        unsigned char getLoops();
        int getMidi1();
        int getMidi2();
        void activateLooperControl(int num);
        void deactivateLooperControl(int num);
        void neutralizeLooperControl(int num);
};

#endif //State_h
