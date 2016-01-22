#include <LightGrid.h>

#define NUM_REGISTERS 1
#define MODE_RED 0 // mode led
#define MODE_GREEN 1 // mode led
#define MODE_BLUE 2 // mode led
#define LED1 0 // loop 1
#define LED2 1 // loop 2
#define LED3 2 // loop 3
#define LED4 3 // loop 4
#define LED5 4 // loop 5
#define LED6 5 // bank bit 1
#define LED7 6 // bank bit 2
#define LED8 7 // bank bit 3

LightGrid::LightGrid(int serPin, int rClockPin, int srClockPin) {
    // Pin mode set by shifter lib
    _shifter = new Shifter(serPin, rClockPin, srClockPin, NUM_REGISTERS);

    clearShifter();
}

void LightGrid::clearShifter() {
    _shifter->clear();
    _shifter->write();
}

void LightGrid::set(unsigned char grid, int red, int green, int blue) {
    if (red != -1) {
        analogWrite(MODE_RED, red);
        analogWrite(MODE_GREEN, blue);
        analogWrite(MODE_BLUE, green);
    }

    int i = 0;

    while (i < 8) {
        if (grid & 0x01) {
            turnOnLed(i);
        }
        else {
            turnOffLed(i);
        }

        i++;
        grid = grid >> 1;
    }
}

void LightGrid::setMode(int red, int green, int blue) {
    analogWrite(MODE_RED, red);
    analogWrite(MODE_GREEN, blue);
    analogWrite(MODE_BLUE, green);
}

void LightGrid::turnOnLed(int num) {
    _shifter->setPin(num, HIGH);
}

void LightGrid::turnOffLed(int num) {
    _shifter->setPin(num, LOW);
}
