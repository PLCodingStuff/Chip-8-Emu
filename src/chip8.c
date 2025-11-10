#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

static const uint8_t chip8_fontset[80] = {
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


Chip8 chip8_init() {
    /*
        Initialize the Chip-8 system by clearing memory, registers,
        stack, keypad, and display. Set the program counter to the start
        address.
    */
    Chip8 chip8;

    memset(&chip8, 0, sizeof(Chip8));

    chip8.pc = START_ADDRESS;
    chip8.screen_updated = 1;

    memcpy(chip8.memory, chip8_fontset, sizeof(chip8_fontset));
    srand(time(NULL));

    return chip8;
}


uint8_t chip8_load_rom(Chip8* chip8, const char *filename) {
    /*
        Load a ROM file into the Chip-8 memory starting at the
        predefined start address.
    */
    FILE *rom = fopen(filename, "rb");
    
    if (rom == NULL) {
        printf("Failed to open ROM: %s\n", filename);
        return 1;
    }

    for (uint16_t byte = START_ADDRESS; byte <= END_ADDRESS; byte++) {
        int read_byte = fgetc(rom);

        if (read_byte == EOF) {
            break;
        }

        chip8->memory[byte] = (uint8_t) read_byte;
    }

    fclose(rom);
    return 0;
}


#define PIXEL(X, x, y) (X)->display[(y)*DISPLAY_WIDTH + (x)] //Dxyn Helper

void DXYN(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t n) {
    uint8_t x_pos = chip8->V[x];
    uint8_t y_pos = chip8->V[y];

    chip8->V[0xF] = 0; // Reset overlap register

    for (uint8_t byte = 0; byte < n; byte++) {
        uint8_t sprite = chip8->memory[chip8->I+byte];

        for (uint8_t col = 0x80; col > 0x0; col >>= 1) {
            uint8_t pixel_prev = PIXEL(chip8, x_pos, y_pos);

            PIXEL(chip8, x_pos, y_pos) ^= sprite & col;
            
            if (pixel_prev && (sprite & col)) // Pixel overflow
                chip8->V[0xF] = 1;
            
            x_pos = (x_pos + 1) % DISPLAY_WIDTH;
        }

        y_pos = (y_pos + 1) % DISPLAY_HEIGHT;
        x_pos = chip8->V[x];
    }
    chip8->screen_updated = 1;
}


Instruction decode_opcode(uint16_t opcode) {
    Instruction instruction;

    instruction.x = (opcode & 0x0F00) >> 8;
    instruction.y = (opcode & 0x00F0) >> 4;
    instruction.byte = opcode & 0x00FF;
    instruction.addr = opcode & 0x0FFF;
    instruction.nibble = opcode & 0x000F;

    instruction.upper_nibble = (opcode & 0xF000) >> 12;

    // Decode Name
    if (instruction.upper_nibble > 0x0 && instruction.upper_nibble < 0x8)
        instruction.name = instruction.upper_nibble + 1; // JP to ADDVb
    else if (instruction.upper_nibble > 0x8 && instruction.upper_nibble < 0xE)
        instruction.name = instruction.upper_nibble + 9; // SNEVV to DRW
    else if (instruction.upper_nibble == 0x0)
        instruction.name = (instruction.nibble != 0); // 0 CLS - 1 RET
    else if (instruction.upper_nibble == 0xE)
        instruction.name = (instruction.byte != 0x9E) + 23; // 23 SKP - 24 SKPN
    else if (instruction.upper_nibble == 0x8 && instruction.nibble != 0xE)
        instruction.name = instruction.nibble + 9; // LDVV to SUBN
    else if (instruction.upper_nibble == 0x8 && instruction.nibble == 0xE)
        instruction.name = SHL;
    else if (instruction.upper_nibble == 0xF) {
        switch (instruction.byte) { //LDVDT to LDVI
            case 0x07:
                instruction.name = LDVDT;
                break;
            case 0x0A:
                instruction.name = LDVK;
                break;
            case 0x15:
                instruction.name = LDDTV;
                break;
            case 0x18:
                instruction.name = LDVST;
                break;
            case 0x1E:
                instruction.name = ADDIV;
                break;
            case 0x29:
                instruction.name = LDFV;
                break;
            case 0x33:
                instruction.name = LDBV;
                break;
            case 0x55:
                instruction.name = LDIV;
                break;
            case 0x65:
                instruction.name = LDVI;
                break;
        }
    }
    else
        instruction.name = UNKWN;

    return instruction;
}


uint16_t chip8_cycle(Chip8 *chip8) {
    /* Fetch, decode, and execute one opcode. */

    // Fetch Opcode
    uint16_t opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    Instruction instruction = decode_opcode(opcode);

    chip8->pc += 2;    
    // Decode Opcode
    switch (instruction.name)
    {
        case CLS:
            memset(chip8->display, 0, sizeof(chip8->display));
            break;
        case RET:
            chip8->pc = chip8->stack[chip8->sp--];
            break;
        case JP:
            chip8->pc = instruction.addr;
            break;
        case CALL:
            chip8->stack[++chip8->sp] = chip8->pc;
            chip8->pc = instruction.addr;
            break;
        case SEVb:
            chip8->pc = chip8->V[instruction.x] == instruction.byte ? chip8->pc + 2 : chip8->pc;
            break;
        case SNEVb:
            chip8->pc = chip8->V[instruction.x] != instruction.byte ? chip8->pc + 2 : chip8->pc;
            break;
        case SEVV:
            chip8->pc = chip8->V[instruction.x] == chip8->V[instruction.y] ? chip8->pc + 2: chip8->pc;
            break;
        case LDVb:
            chip8->V[instruction.x] = instruction.byte;
            break;
        case ADDVb:
            chip8->V[instruction.x] += instruction.byte;
            break;
        case LDVV:
            chip8->V[instruction.x] = chip8->V[instruction.y];
            break;
        case OR:
            chip8->V[instruction.x] |= chip8->V[instruction.y];
            break;
        case AND:
            chip8->V[instruction.x] &= chip8->V[instruction.y];
            break;
        case XOR:
            chip8->V[instruction.x] ^= chip8->V[instruction.y];
            break;
        case ADDVV:
            uint16_t sum = chip8->V[instruction.x] + chip8->V[instruction.y];
            chip8->V[0xF] = sum > 0xFF ? 1 : 0;
            chip8->V[instruction.x] = sum & 0xFF;
            break;
        case SUB:
            chip8->V[0xF] = chip8->V[instruction.x] > chip8->V[instruction.y] ? 1 : 0;
            chip8->V[instruction.x] -= chip8->V[instruction.y];
            break;
        case SHR:
            chip8->V[0xF] = chip8->V[instruction.x] & 0x1;
            chip8->V[instruction.x] >>= 1;
            break;
        case SUBN:
            chip8->V[0xF] = chip8->V[instruction.x] > chip8->V[instruction.y] ? 1 : 0;
            chip8->V[instruction.x] = chip8->V[instruction.y] - chip8->V[instruction.x];
            break;
        case SHL:
            chip8->V[0xF] = chip8->V[instruction.x] >> 7;
            chip8->V[instruction.x] <<= 1;
            break;
        case SNEVV:
            chip8->pc = chip8->V[instruction.x] != chip8->V[instruction.y] ? chip8->sp+2 : chip8->sp;
            break;
        case LDI:
            chip8->I = instruction.addr;
            break;
        case JPV0:
            chip8->pc = instruction.addr + chip8->V[0x0];
            break;
        case RND:
            chip8->V[instruction.x] = rand() % 255 & instruction.byte;
            break;
        case DRW:
            DXYN(chip8, instruction.x, instruction.y, instruction.nibble);
            break;
        case SKP:
            ;
            break;
        case SKNP:
            ;
            break;
        case LDVDT:
            chip8->V[instruction.x] = chip8->DT;
            break;
        case LDVK:
            ;
            break;
        case LDDTV:
            chip8->DT = chip8->V[instruction.x];
            break;
        case LDVST:
            chip8->ST = chip8->V[instruction.x];
            break;
        case ADDIV:
            chip8->I += chip8->V[instruction.x];
            break;
        case LDFV:
            ;
            break;
        case LDBV:
            chip8->memory[chip8->I] = chip8->V[instruction.x] % 1000;
            chip8->memory[chip8->I+1] = chip8->V[instruction.x] % 100;
            chip8->memory[chip8->I+2] = chip8->V[instruction.x] % 10;
            break;
        case LDIV:
            for(uint8_t reg = 0; reg < instruction.x; reg++)
                chip8->memory[chip8->I+reg] = chip8->V[reg];
            break;
        case LDVI:
            for(uint8_t reg = 0; reg < instruction.x; reg++)
                chip8->V[reg] = chip8->memory[chip8->I+reg];
            break;
        default:
            printf("ERROR: Unknown opcode: %4u\n", opcode);
            break;
    }
    return opcode;
}


void chip8_print_display(Chip8* chip) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint16_t pixel = chip->display[y * DISPLAY_WIDTH + x];
            putc(pixel ? '*' : ' ', stdout);
        }
        putc('\n', stdout);
    }
}