#ifndef Buttons_h
#define Buttons_h

#include <Arduino.h>
#include <Shifter.h>
#include <math.h>

#define BUTTON_COUNT 10

struct button {
    long time;
    boolean state;
    boolean prevState;
    boolean press;
    boolean pressed;
    boolean pressHold;
    boolean release;
};

const struct button initButton = {
    -1.0, 0, 0, 0, 0, 0, 0
};

class Buttons
{
    private:
        struct button buttons[BUTTON_COUNT];
    public:
        Buttons();
        void updateStates(unsigned char states);
        boolean* getStates();
        void updateState(int num, boolean state);
        boolean** detectEvents();
};

#endif //Buttons_h
