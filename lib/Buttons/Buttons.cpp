#include <Buttons.h>

#define BUTTON_COUNT 6

Buttons::Buttons() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i] = initButton;
    }
}

int detectEvent(int num) {
    struct button button;
    struct button leftButton;
    struct button rightButton;

    for (int i = 0; i < BUTTON_COUNT; i++) {
        button = buttons[i];
        leftButton = null;
        rightButton = null;
        if (i > 0) {
            leftButton = buttons[i - 1];
        }
        if (i < BUTTON_COUNT - 2) {
            rightButton = buttons[i + 1];
        }

        // note the time
        if (!button.prevState && button.state) {
            button.time = millis();
        }

        // detect press
        if (button.state && button.time != -1 && millis() - button.time > 50) {
            if (!button.pressed && !button.press) {
                button.press = true;
            }
            if (!button.pressed && button.press) {
                button.press = false;
                button.pressed = true;
            }
        }

        // detect presshold
        if (button.press && button.state && millis() - button.time > 1000) {
            button.pressHold = true;
        }

        // reinit button once its released
        if (button.prevState && !button.state) {
            button.time = -1
        }

        if (!button.press && !button.pressed && button.time != -1) {
            if (rightButton.time != -1) {
                // trigger double press self and to the right
                button.press = true;
                rightButton.press = true;
            }
            if (leftButton.time != -1) {
                // trigger double press self and to the left
                button.press = true;
                leftButton.press = true;
                // TODO ADD DOUBLE PRESS BUTTONS!!!!!
            }
        }
    }
}
