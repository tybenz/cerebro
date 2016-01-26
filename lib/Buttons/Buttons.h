#ifndef Buttons_h
#define Buttons_h

#include <Arduino.h>
#include <Shifter.h>
#include <math.h>

#define BUTTON_COUNT 10

struct button {
    long time;
    bool state;
    bool prevState;
    bool press;
    bool pressed;
    bool pressHold;
    bool pressedHeld;
    bool release;
};

const struct button initButton = {
    -1.0, 0, 0, 0, 0, 0, 0, 0
};

class Buttons
{
    private:
        struct button buttons[BUTTON_COUNT];
    public:
        Buttons();
        void updateStates(bool *states);
        bool* getStates();
        void updateState(int num, bool state);
        void detectEvents(bool *presses, bool *pressHolds, bool *releases);
};

#endif //Buttons_h
