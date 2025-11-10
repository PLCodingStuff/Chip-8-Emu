#ifndef CHIP_8_H
#define CHIP_8_H

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16

#define START_ADDRESS 0x200
#define END_ADDRESS 0xFFF
#define INTERPRETER_START 0x000
#define INTERPRETER_END 0x1FF

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define KEYPAD_SIZE 16

#include <stdint.h>

// Chip-8 System Structure
typedef struct {
    uint8_t memory[MEMORY_SIZE];

    // Registers
    uint8_t V[REGISTER_COUNT];
    uint16_t I;
    uint8_t DT;
    uint8_t ST;

    // Pseudo-registers (respresnted with small letters)
    uint16_t pc;
    uint16_t sp;

    uint16_t stack[STACK_SIZE];

    uint8_t keypad[KEYPAD_SIZE];

    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];

    uint8_t screen_updated;
} Chip8;

typedef enum {
    CLS,
    RET,
    JP,
    CALL,
    SEVb,
    SNEVb,
    SEVV,
    LDVb,
    ADDVb,
    LDVV,
    OR,
    AND,
    XOR,
    ADDVV,
    SUB,
    SHR,
    SUBN,
    SHL,
    SNEVV,
    LDI,
    JPV0,
    RND,
    DRW,
    SKP,
    SKNP,
    LDVDT,
    LDVK,
    LDDTV,
    LDVST,
    ADDIV,
    LDFV,
    LDBV,
    LDIV,
    LDVI,
    UNKWN
} Instructions;


typedef struct {
    uint16_t addr;
    uint8_t nibble;
    uint8_t x;
    uint8_t y;
    uint8_t byte;

    uint8_t upper_nibble;

    Instructions name;
} Instruction;

// opcode_enum custom_opcode = process(upper_nibble, byte)
// switch(custom_opcode)
// case JP: //JP belongs in `opcode_enum` type.

// Chip-8 Initialization
Chip8 chip8_init();

Instruction decode_opcode(uint16_t opcode);

// Load ROM into memory
uint8_t chip8_load_rom(Chip8* chip8, const char* filename);
// Emulate one cycle
uint16_t chip8_cycle(Chip8* chip8);


void chip8_print_display(Chip8* chip);

#endif