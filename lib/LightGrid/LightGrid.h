#ifndef LightGrid_h
#define LightGrid_h

#include <Arduino.h>
#include <Shifter.h>

class LightGrid
{
    public:
        LightGrid(int serPin, int rClockPin, int srClockPin);
        void clearShifter();
        void set(unsigned char grid, int red, int green, int blue);
        void setMode(int red, int green, int blue);
        void turnOffAll();
        void turnOnLed(int num);
        void turnOffLed(int num);
        void turnOnBankLed(int num);
        void turnOffBankLed(int num);
        void turnOnPatchLed(int num);
        void turnOffPatchLed(int num);

    private:
        Shifter* _shifter;
};

#endif //LightGrid_h
