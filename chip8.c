#include<stdio.h>
#include<stdint.h>

struct Chip8 {
    // 4kb of memory
    uint8_t memory[4096];
    // 16 8-bit registers (V0 - VF)
    uint8_t V[16];
    // index register
    uint8_t index;
    // timers
    uint8_t delay_timer;
    uint8_t sound_timer;
    // program counter (PC)
    uint16_t pc;
    // stack
    uint16_t stack[16];
    // stack pointer
    uint8_t sp;
    // display
    uint32_t display[64][32];
    // 16bit opcode
    uint16_t opcode;
};