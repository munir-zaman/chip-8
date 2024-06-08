#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

const unsigned int START_ADDRESS = 0x200;

typedef struct Chip8 {
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
} Chip8;

Chip8 chip8;

int load_rom(char *filename) {
    FILE *fp;
    fp = fopen(filename, "rb");

    if (fp == NULL) {
        printf("Error : failed to load file \'%s\'\n", filename);
        return -1;
    }

    // calculate size of file -> f_size
    fseek(fp, 0L, SEEK_END);
    long int f_size = ftell(fp);

    // reset position
    fseek(fp, 0L, SEEK_SET);

    // load to memory
    unsigned char x;
    for (int i = 0; ((x = fgetc(fp)) != EOF); i++) {
        chip8.memory[START_ADDRESS + i] = x;
    }

    // close and return
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_rom(argv[1]);
    }
}