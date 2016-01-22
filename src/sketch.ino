#include <LightGrid.h>
#include <Storage.h>
#include <State.h>

LightGrid* lightgrid = new LightGrid(10, 11, 12);
Storage* storage = new Storage();
State* state = new State();

void setup() {
    Serial.begin(31250);

    storage->saveState(0, 0x01, 0x01, 0x01, 23);
    storage->savePresetByBankAndPatch(1, 1, 0x01, 0x01, 0x01);

    unsigned char loops = 0x08;
    state->setState(1, 5, 1, 1, loops);
}

void loop() {
    unsigned char grid = 0x01;
    lightgrid->set(grid, -1, -1, -1);
    // check buttons
    // update state
    // re-render
    // write to storage if anything has changed
}
