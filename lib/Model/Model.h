#ifndef Model_h
#define Model_h

#include <Arduino.h>

class Model
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
        int getPatchForPresetNum(int num);
        Model();
        void setModel(int newPreset, int newMidi1, int newMidi2, unsigned char newLoops, int newLooper[]);
        bool diff(Model* model);
        Model* copy();
        void midi1Up();
        void midi1Down();
        void midi2Up();
        void midi2Down();
        void bankUp();
        void bankDown();
        void clearTempBank();
        int getTempBank();
        int selectPatchByNum(int num);
        int selectPatch(int num);
        int selectPatch(int num, bool useTempBank);
        void activateLoop(int num);
        void deactivateLoop(int num);
        void toggleLoop(int num);
        void setLoops(unsigned char loops);
        int getPresetNum(int num);
        int getTempPresetNum(int num);
        unsigned char getLoops();
        int getMidi1();
        int getMidi2();
        void activateLooperControl(int num);
        void deactivateLooperControl(int num);
        void neutralizeLooperControl(int num);
        void setMidi1(int num);
        void setMidi2(int num);
};

#endif //Model_h
