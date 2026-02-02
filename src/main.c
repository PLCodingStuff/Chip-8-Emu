#include "chip8.h"
#include <stdio.h>

#define TERMINATION_REPS 3

int main(int argc, char* argv[]) {
    Chip8 emu = chip8_init();

    uint16_t opcode, prev_opcode = 0x0000;
    uint8_t opcode_reps = 0;
    if (argc < 2) {
        printf("Error: No ROM file specified.\n");
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];

    if (chip8_load_rom(&emu, filename))
        return 1;

    for (int i = 0; i < 1000; i++) {
        opcode = chip8_cycle(&emu);

        // Updated Screen
        if(emu.screen_updated) {
            chip8_print_display(&emu);
            emu.screen_updated = 0;
        }
        if (prev_opcode == opcode)
            opcode_reps++;

        prev_opcode = opcode;

        if (opcode_reps > TERMINATION_REPS)
            break;
    }

    return 0;
}