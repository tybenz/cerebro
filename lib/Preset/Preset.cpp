#include <Preset.h>

Preset::Preset() : loops(0x00), midi1(0x00), midi2(0x00) {
}

Preset::Preset(unsigned char loops, int midi1, int midi2) {
    loops = loops;
    midi1 = midi1;
    midi2 = midi2;
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
