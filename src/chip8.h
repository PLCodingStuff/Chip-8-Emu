#ifndef CHIP_8_H
#define CHIP_8_H

#define MEMORY_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16

#define START_ADDRESS 0x200
#define END_ADDRESS 0xFFF
#define INTERPRETER_START 0x000
#define INTERPRETER_END 0x1FF

// Chip-8 System Structure
typedef struct {
    unsigned char memory[MEMORY_SIZE];

    // Registers
    unsigned char V[REGISTER_COUNT];
    unsigned short I;
    unsigned char DT;
    unsigned char ST;

    // Pseudo-registers (respresnted with small letters)
    unsigned short pc;
    unsigned short sp;

    unsigned short stack[STACK_SIZE];

    unsigned char keypad[16];

    unsigned char display[64 * 32];
} chip8_t;


// Chip-8 Initialization
chip8_t chip8_init();
// Load ROM into memory
unsigned char chip8_load_rom(chip8_t* chip8, const char* filename);
// Emulate one cycle
void chip8_cycle(chip8_t* chip8);


#endif