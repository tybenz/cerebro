#ifndef LightGrid_h
#define LightGrid_h

#include <Arduino.h>
#include <Shifter.h>

#define NUM_LEDS 9

class LightGrid
{
    public:
        LightGrid(int serPin, int rClockPin, int srClockPin, int lastPin);
        void clearShifter();
        void writeShifter();
        void set(bool leds[NUM_LEDS], int red, int green, int blue);
        void setMode(int red, int green, int blue);
        void setLeds(bool leds[NUM_LEDS]);
        void turnOffAll();
        void turnOnLed(int num);
        void turnOnLed(int num, int offset);
        void turnOffLed(int num);
        void turnOffLed(int num, int offset);
        void turnOnBankLed(int num);
        void turnOffBankLed(int num);
        void turnOnPatchLed(int num);
        void turnOffPatchLed(int num);

    private:
        Shifter* _shifter;
        int _lastPin;
};

#endif //LightGrid_h
