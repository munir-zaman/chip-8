#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

#define DEBUG 1

// extracting nibbles/hex values for opcode
#define HEX(opcode, index) (opcode & (0xF << (4*index))) >> (4*index)

// address of start position
const unsigned int START_ADDRESS = 0x200;
const uint32_t DEFAULT_COLOR = 0xFFFFFFFF;

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
    // 16-bit opcode
    uint16_t opcode;
} Chip8;

Chip8 chip8;

// load a rom file into the memory of chip8 (chip8.memory)
int load_rom(char *filename) {
    // open file
    FILE *fp;
    // rb -> read + binary (read the file in binary)
    fp = fopen(filename, "rb");

    // check for file errors
    if (fp == NULL) {
        printf("Error : failed to load file \'%s\'\n", filename);
        return -1;
    }

    // calculate size of file -> f_size
    fseek(fp, 0L, SEEK_END);
    long int f_size = ftell(fp);
    if (DEBUG) printf("File size of %s : %d", filename, f_size);

    // reset position
    fseek(fp, 0L, SEEK_SET);

    // load to memory -> chip8.memory
    unsigned char x;
    for (int i = 0; i<f_size; i++) {
        x = fgetc(fp);
        chip8.memory[START_ADDRESS + i] = x;
        if (DEBUG) printf("%d %02X\n", i, x);
    }

    // NOTE: " (x = fgetc(fp)) != EOF " causes a bug.. probably because of type conversion

    // close and return
    fclose(fp);
    return 0;
}

void execute_instruction(uint16_t opcode) {
    // an opcode is 16 bits
    // last_digit is going to store the last nibble or hex value
    uint8_t last_digit = HEX(opcode, 3);
    if (DEBUG) printf("Opcode Type : %02X\n", last_digit);
    if (last_digit == 0x0) {
        // 00E0 - CLS
        if (HEX(opcode, 1) == 0xE && HEX(opcode, 0) == 0x0) {
            for (int i = 0; i < 64; i++)
                for (int j = 0; j < 32; j++)
                    chip8.display[i][j] = DEFAULT_COLOR;

            if (DEBUG) printf("Cleared The Screen");
        }
        // 00EE - RET
        else if (HEX(opcode, 1) == 0xE && HEX(opcode, 0) == 0xE) {
            chip8.pc = chip8.stack[chip8.sp];
            chip8.sp--;
            if (DEBUG) printf("Executed RET.. returned to %04X", chip8.pc);
        }
    }
}

void cycle() {
    // Fetch the opcode

    // Decode and execute the instruction

    // Update the program counter and timers

    // TODO
}

int main(int argc, char *argv[]) {
    chip8.pc = START_ADDRESS;
    if (argc > 1) {
        int err = load_rom(argv[1]);
        printf(err);
        execute_instruction(0x00E0);
    }

}