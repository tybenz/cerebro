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
};

const struct button initButton = {
    -1.0, 0, 0, 0, 0, 0
};

class Buttons
{
    private:
        struct button buttons[BUTTON_COUNT];
    public:
        Buttons();
        void updateStates(unsigned char states);
        void updateState(int num, boolean state);
        int detectEvent(int num);
};

#endif //Buttons_h
