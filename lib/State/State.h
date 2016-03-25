#ifndef State_h
#define State_h

#include <Arduino.h>

class State
{
    public:
        int currentPreset;
        int currentSetlistPreset;
        int tempBank;
        int setlistTempBank;
        int currentAbleton;
        int midi1;
        int midi2;
        int looper[3];
        unsigned char loops;
        int getBank();
        int getSetlistBank();
        int getPatch();
        int getPatchForPresetNum(int num);
        State();
        void setState(int newPreset, int newSetlistPreset, int newMidi1, int newMidi2, unsigned char newLoops, int newLooper[]);
        bool diff(State* state);
        State* copy();
        void midi1Up();
        void midi1Down();
        void midi2Up();
        void midi2Down();
        void bankUp();
        void bankDown();
        void clearTempBank();
        void setlistBankUp();
        void setlistBankDown();
        void clearSetlistTempBank();
        int selectSetlistPatchByNum(int num);
        int selectSetlistPatch(int num);
        int selectSetlistPatch(int num, bool useTempBank);
        int getTempBank();
        int getSetlistTempBank();
        int selectPatchByNum(int num);
        int selectPatch(int num);
        int selectPatch(int num, bool useTempBank);
        void activateLoop(int num);
        void deactivateLoop(int num);
        void toggleLoop(int num);
        void setLoops(unsigned char loops);
        int getPresetNum(int num);
        int getTempPresetNum(int num);
        int getTempSetlistPresetNum(int num);
        unsigned char getLoops();
        int getMidi1();
        int getMidi2();
        void activateLooperControl(int num);
        void deactivateLooperControl(int num);
        void neutralizeLooperControl(int num);
        void setMidi1(int num);
        void setMidi2(int num);
        void abletonPlay();
        void abletonUp();
        void abletonStop();
        void abletonPlay(int num);
        void abletonSelectUp();
        void abletonSelectDown();
};

#endif //State_h
