#include <Buttons.h>

#define BUTTON_COUNT 10
#define ACTUAL_BUTTON_COUNT 6
#define START_INDEX_FOR_SIDE_BY_SIDE_BUTTONS 2
#define END_INDEX_FOR_SIDE_BY_SIDE_BUTTONS 5

Buttons::Buttons() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i] = initButton;
    }
}

void Buttons::updateStates(unsigned char states) {
    // states is a byte where first 6 bits reflect on/off of buttons
    // ex: 00001100, means mode is off, button 1 off, button 2 on, button 3 on,
    // button 4 off, button 5 off
    for (int i = 2; i < 8; i++) {
        struct button button = buttons[i-2];
        button.prevState = button.state;
        if(states & (1 << i)) {
            button.state = true;
        } else {
            button.state = false;
        }
    }
}

void Buttons::updateState(int num, boolean state) {
    struct button button = buttons[num];
    button.prevState = button.state;
    button.state = state;
}

boolean** Buttons::detectEvents() {
    boolean presses[BUTTON_COUNT];
    boolean pressHolds[BUTTON_COUNT];
    boolean* returnArr[2] = { presses, pressHolds };
    int i;

    for (i = 0; i < BUTTON_COUNT; i++) {
        presses[i] = 0x00;
        pressHolds[i] = 0x00;
    }

    struct button button;
    struct button leftButton;

    for (i = 0; i < BUTTON_COUNT; i++) {
        button = buttons[i];
        leftButton = initButton;

        // Buttons should be
        // 0(mode)         1 <-6-> 2 <-7-> 3 <-8-> 4 <-9-> 5
        //     start checking left ^    stop chekcing left ^
        if (i > START_INDEX_FOR_SIDE_BY_SIDE_BUTTONS &&
            i <= END_INDEX_FOR_SIDE_BY_SIDE_BUTTONS) {
            leftButton = buttons[i - 1];
        }

        // note the time
        if (!button.prevState && button.state) {
            button.time = millis();
        }

        // detect press
        // if enough time has passed, trigger a press
        // if the press has already been triggered switch it to "pressed"
        // so the press only fires 1 time
        if (button.state && button.time != -1 && millis() - button.time > 50) {
            if (!button.pressed && !button.press) {
                button.press = true;
                presses[i] = true;
            }
            if (!button.pressed && button.press) {
                button.press = false;
                button.pressed = true;
            }
        }

        // detect presshold
        // if button is pressed for 1 sec, it's a pressHold
        if (button.pressed && button.state && millis() - button.time > 1000) {
            button.pressHold = true;
            pressHolds[i] = true;
        }

        // reinit button once its released
        if (button.prevState && !button.state) {
            // if it pressed and released with 50 ms, trigger press
            if (millis() - button.time <= 50) {
                button.press = true;
            }
            button.time = -1;
        }

        // Catch the press+release within 50ms case
        if (!button.prevState && !button.state && button.press) {
            button.press = false;
        }

        // if button state is HIGH for first 50 ms, time will be set, but press
        // and pressed with be false
        if (button.time != -1 && !button.press && !button.pressed) {
            if (leftButton.time != -1 && !leftButton.press && !leftButton.pressed) {
                // trigger double press self and to the left
                button.press = true;
                leftButton.press = true;
                // set up double press (always 4 higher)
                buttons[i+4].time = fmin(button.time, leftButton.time);
                buttons[i+4].press = true;
                buttons[i+4].state = HIGH;
            }
        }
    }

    return returnArr;
}
