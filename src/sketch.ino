#include <LightGrid.h>
#include <Storage.h>
#include <State.h>

LightGrid* lightgrid = new LightGrid(10, 11, 12);
Storage* storage = new Storage();
State* state = new State();

#define LIVE 0
#define MIDI 1
#define PRESET 2
#define LOOPER 3
int mode = 0;


void setup() {
    Serial.begin(31250);
    // read state from EEPROM
    // set state object and render
}

void loop() {
    // check buttons
    // update state
    // re-render
    // write to storage if anything has changed
}

void nextMode() {
    mode++;
    if (mode > 3) {
        mode = LIVE;
    }
}
