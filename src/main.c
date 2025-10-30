#include "chip8.h"

int main(void) {
    chip8_t emu = chip8_init();

    char *filename = "./roms/IBM Logo.ch8";

    if (!chip8_load_rom(&emu, filename)) {
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        chip8_cycle(&emu);
    }

    return 0;
}