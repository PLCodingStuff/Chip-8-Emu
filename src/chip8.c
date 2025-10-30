#include "chip8.h"
#include <stdio.h>
#include <string.h>

static const unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


chip8_t chip8_init() {
    /*
        Initialize the Chip-8 system by clearing memory, registers,
        stack, keypad, and display. Set the program counter to the start
        address.
    */
    chip8_t chip8;

    memset(&chip8, 0, sizeof(chip8_t));

    chip8.pc = START_ADDRESS;


    memcpy(chip8.memory, chip8_fontset, sizeof(chip8_fontset));
    return chip8;
}


unsigned char chip8_load_rom(chip8_t* chip8, const char *filename) {
    /*
        Load a ROM file into the Chip-8 memory starting at the
        predefined start address.
    */
    FILE *rom = fopen(filename, "rb");

    if (rom == NULL) {
        printf("Failed to open ROM: %s\n", filename);
        return 1;
    }

    for (unsigned short byte = START_ADDRESS; byte <= END_ADDRESS; byte++) {
        int read_byte = fgetc(rom);

        if (read_byte == EOF) {
            break;
        }

        chip8->memory[byte] = (unsigned char)read_byte;
    }

    fclose(rom);
    return 0;
}

// Helper functions for opcodes
void CLS(chip8_t* chip8) {
    /*
        Clear the display by setting all pixels to 0.
    */
    memset(chip8->display, 0, sizeof(chip8->display));
}

void JP(chip8_t* chip8, unsigned short address) {
    /*
        Jump to the specified address by setting the program counter.
    */
    chip8->pc = address;
}

void LDVb(chip8_t *chip8, unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00) >> 8;
    unsigned char byte = opcode & 0x00FF;

    chip8->V[x] = byte;
}

void ADDVb(chip8_t *chip8, unsigned short opcode){
    unsigned char x = (opcode & 0x0F00) >> 8;
    unsigned char byte = opcode & 0x00FF;

    chip8->V[x] += byte;
}

void LDI(chip8_t *chip8, unsigned short opcode){
    unsigned short address = opcode & 0x0FFF;

    chip8->I = address;
}


void chip8_cycle(chip8_t* chip8) {
    /*
    Fetch, decode, and execute one opcode cycle.
    Currently only prints the fetched opcode in hex.
    */

    // Fetch Opcode
    unsigned short opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    chip8->pc += 2;

    // printf("Opcode: %04X\n", opcode);
    // Decode Opcode

    switch (opcode & 0xF000)
    {
    case 0x0000:
        // handle 0x00E0 and 0x00EE
        if (opcode == 0x00E0)
            CLS(chip8);
        break;
    case 0x1000:
        JP(chip8, opcode & 0x0FFF);
        break;
    case 0x6000:
        LDVb(chip8, opcode);
        break;
    case 0x7000:
        ADDVb(chip8, opcode);
        break;
    case 0xA000:
        LDI(chip8, opcode);
        break;
    default:
        printf("Opcode not yet implememnted: %04X\n", opcode);
    }
    
}