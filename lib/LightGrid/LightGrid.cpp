#include <LightGrid.h>

#define NUM_LEDS 9
int MODE_RED = A0;
int MODE_GREEN = A1;
int MODE_BLUE = A2;

LightGrid::LightGrid(int serPin, int rClockPin, int srClockPin, int lastPin) {
    // Pin mode set by shifter lib
    _shifter = new Shifter(serPin, rClockPin, srClockPin, 1);
    _lastPin = lastPin;
    pinMode(_lastPin, OUTPUT);

    clearShifter();
}

void LightGrid::writeShifter() {
    _shifter->write();
}

void LightGrid::clearShifter() {
    _shifter->clear();
    _shifter->write();
}

void LightGrid::set(bool* leds, int red, int green, int blue) {
    setLeds(leds);
    setMode(red, green, blue);
}

void LightGrid::setLeds(bool* leds) {
    _shifter->clear();
    for (int i = 0; i < NUM_LEDS; i++) {
        if (leds[i]) {
            turnOnLed(i);
        } else {
            turnOffLed(i);
        }
    }
    _shifter->write();
}

void LightGrid::setMode(int red, int green, int blue) {
    if (red != -1) {
        analogWrite(MODE_RED, red);
        analogWrite(MODE_GREEN, green);
        analogWrite(MODE_BLUE, blue);
    }
}

void LightGrid::turnOffAll() {
    _shifter->clear();
    for (int i = 0; i < NUM_LEDS; i++) {
        turnOffLed(i);
    }
    _shifter->write();
}

void LightGrid::turnOnLed(int num) {
    if (num == 8) {
        digitalWrite(_lastPin, HIGH);
    } else {
        _shifter->setPin(num, HIGH);
    }
}

void LightGrid::turnOnLed(int num, int offset) {
    if (num == 8) {
        digitalWrite(_lastPin, HIGH);
    } else {
        _shifter->setPin(num + offset, HIGH);
    }
}

void LightGrid::turnOffLed(int num) {
    if (num == 8) {
        digitalWrite(_lastPin, LOW);
    } else {
        _shifter->setPin(num, LOW);
    }
}

void LightGrid::turnOffLed(int num, int offset) {
    if (num == 8) {
        digitalWrite(_lastPin, LOW);
    } else {
        _shifter->setPin(num + offset, LOW);
    }
}

void LightGrid::turnOnBankLed(int num) {
    turnOnLed(num, 5);
}

void LightGrid::turnOffBankLed(int num) {
    turnOffLed(num, 5);
}

void LightGrid::turnOnPatchLed(int num) {
    turnOnLed(num, 2);
}

void LightGrid::turnOffPatchLed(int num) {
    turnOffLed(num, 2);
}
