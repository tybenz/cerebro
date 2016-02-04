#include <Preset.h>

Preset::Preset(unsigned char initLoops, int initMidi1, int initMidi2) {
    loops = initLoops;
    midi1 = initMidi1;
    midi2 = initMidi2;
}

unsigned char Preset::getLoops() {
    return loops;
}

int Preset::getMidi1() {
    return midi1;
}

int Preset::getMidi2() {
    return midi2;
}
