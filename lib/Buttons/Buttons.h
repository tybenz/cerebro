#ifndef Buttons_h
#define Buttons_h

#include <Arduino.h>
#include <Shifter.h>

#define BUTTON_COUNT 6

struct button {
    long time
    boolean state
    boolean prevState
    boolean press
    boolean pressed
    boolean pressHold
}

const struct initButton = {
    -1, 0, 0, 0, 0, 0
}

class Buttons
{
    private:
        struct button buttons[BUTTON_COUNT];
    public:
        Buttons();
        int detectEvent(int num);
}

#endif //Buttons_h
