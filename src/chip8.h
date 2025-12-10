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
#include <stdbool.h>


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
} Chip8;

// Chip-8 Initialization
Chip8 chip8_init();

// Load ROM into memory
bool chip8_load_rom(Chip8* emu, const char* filename);

// Fetch the instruction from memory
uint16_t chip8_fetch(Chip8* emu);

// Decode the operation code of the instruction
Instruction chip8_decode(uint16_t opcode);

bool chip8_execute(Chip8* emu, Instruction instruction);

void chip8_quit(Chip8* emu);



#endif