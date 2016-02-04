#include <Buttons.h>

#define BUTTON_COUNT 10
#define ACTUAL_BUTTON_COUNT 6
#define START_INDEX_FOR_SIDE_BY_SIDE_BUTTONS 2
#define END_INDEX_FOR_SIDE_BY_SIDE_BUTTONS 5

int thresholds[6][2] = {
  {575, 600},
  {525, 540},
  {445, 470},
  {350, 380},
  {215, 250},
  {1, 30}
};

Buttons::Buttons() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        buttons[i] = initButton;
    }
}

void Buttons::updateStates(int num) {
    bool show = num < 1000;
    for (int i = 0; i < ACTUAL_BUTTON_COUNT; i++) {
        int* threshold = thresholds[i];
        struct button button = buttons[i];
        bool prevState = button.state;
        if (num >= threshold[0] && num <= threshold[1]) {
            button.state = true;
        } else {
            button.state = false;
        }
        button.prevState = prevState;
        buttons[i] = button;
    }
}

int prevTime = 0;
bool prevPress = false;
bool prevPressed = false;
bool prevRelease = false;

void Buttons::detectEvents(bool *presses, bool *pressHolds, bool *releases) {
    int i;

    struct button button;
    struct button leftButton;

    for (i = 0; i < ACTUAL_BUTTON_COUNT; i++) {
        button = buttons[i];

        if (i == 0) {
            if (prevTime != button.time ||
                prevPress != button.press ||
                prevPressed != button.pressed ||
                prevRelease != button.release ||
                button.prevState != button.state) {
            }
        }

        // finish reinitialization of a button by stetting release to false
        if (!button.prevState && !button.state && button.release) {
            button.release = false;
        }

        // note the time when a button is first held down
        if (!button.prevState && button.state && button.time == -1) {
            button.time = millis();
        }

        // detect press
        // if enough time has passed, trigger a press
        // if the press has already been triggered switch it to "pressed"
        // so the press only fires 1 time
        if (button.state && button.time != -1 && ( millis() - button.time ) > 50) {
            if (!button.pressed && !button.press) {
                button.press = true;
                presses[i] = true;
            } else if (!button.pressed && button.press) {
                button.press = false;
                button.pressed = true;
            }
        }

        // detect presshold
        // if button is pressed for 1 sec, it's a pressHold
        if (button.pressed && button.state && millis() - button.time > 1000) {
            if (!button.pressedHeld && !button.pressHold) {
                button.pressHold = true;
                pressHolds[i] = true;
            } else if (!button.pressedHeld && button.pressHold) {
                button.pressHold = false;
                button.pressedHeld = true;
            }
        }

        // reinit button once its released
        if (button.prevState && !button.state) {
            button.release = true;
            releases[i] = true;
            // if it pressed and released with 50 ms, trigger press
            if (!button.pressed && !button.press) {
                // XXX release will also be set in this case
                // But releases are only used for looper control so immediate
                // press + release is okay for now
                button.press = true;
                presses[i] = true;
            } else {
                // reset everything but release (will get reset next tick)
                button.time = -1;
                button.pressed = false;
                button.pressedHeld = false;
                button.pressHold = false;
                button.press = false;
            }
        }

        // Reset after the press+release within 50ms case
        if (!button.prevState && !button.state && button.press) {
            // reset everything
            button.press = false;
            button.time = -1;
            button.pressed = false;
            button.pressedHeld = false;
            button.pressHold = false;
        }

        // Buttons should be
        // 0(mode)         1 <-6-> 2 <-7-> 3 <-8-> 4 <-9-> 5
        //     start checking left ^    stop chekcing left ^
        // if (i > START_INDEX_FOR_SIDE_BY_SIDE_BUTTONS &&
        //     i <= END_INDEX_FOR_SIDE_BY_SIDE_BUTTONS) {
        //     leftButton = buttons[i - 1];
        // } else {
        //     leftButton = initButton;
        // }

        // if button state is HIGH for first 50 ms, time will be set, but press
        // and pressed with be false
        // if (button.time != -1 && !button.press && !button.pressed) {
        //     if (leftButton.time != -1 && !leftButton.press && !leftButton.pressed) {
        //         // trigger double press self and to the left
        //         button.press = true;
        //         leftButton.press = true;
        //         // set up double press (always 4 higher)
        //         buttons[i+4].time = fmin(button.time, leftButton.time);
        //         buttons[i+4].press = true;
        //         buttons[i+4].state = HIGH;
        //     }
        // }

        if (i == 0) {
            prevTime = button.time;
            prevPress = button.press;
            prevPressed = button.pressed;
            prevRelease = button.release;
        }
        buttons[i] = button;
    }
}