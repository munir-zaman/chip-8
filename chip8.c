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

    uint16_t nnn = (opcode & 0x0FFF);
    uint16_t kk = (opcode & 0x00FF);
    uint16_t x = HEX(opcode, 2);
    uint16_t y = HEX(opcode, 1);
    uint16_t n = HEX(opcode, 0);

    if (last_digit == 0x0) {
        // 00E0 - CLS
        if (opcode == 0x00E0) {
            for (int i = 0; i < 64; i++)
                for (int j = 0; j < 32; j++)
                    chip8.display[i][j] = DEFAULT_COLOR;
        }
        // 00EE - RET
        else if (opcode == 0x00EE) {
            chip8.pc = chip8.stack[chip8.sp];
            chip8.sp--;
            chip8.pc += 2; // move to next instruction
        }
        // 0nnn ?? 
        // Not necessary
    }
    else if (last_digit == 0x1) {
        // JP addr
        chip8.pc = nnn;
    }
    else if (last_digit == 0x2) {
        // CALL addr
        chip8.sp++;
        chip8.stack[chip8.sp] = chip8.pc;
        chip8.pc = nnn;
    }
    else if (last_digit == 0x3) {
        // 3xkk - SE Vx, byte
        // Skip next instruction if Vx = kk.
        if (chip8.V[x] == kk) {
            chip8.pc += 2;
        }
    }
    else if (last_digit == 0x4) {
        if (chip8.V[x] != kk) {
            chip8.pc += 2;
        }
    }
    else if (last_digit == 0x5) {
        if (chip8.V[x] == chip8.V[y]) chip8.pc += 2;
    }
    else if (last_digit == 0x6) {
        chip8.V[x] = kk;
    }
    else if (last_digit == 0x7) {
        chip8.V[x] += kk;
    }
    else if (last_digit == 0x8) {
        if (n == 0x1) {
            chip8.V[x] |= chip8.V[y];
        }
        else if (n == 0x2) {
            chip8.V[x] &= chip8.V[y];
        }
        else if (n == 0x3) {
            chip8.V[x] ^= chip8.V[y];
        }
        else if (n == 0x4) {
            // check for overflow
            uint16_t vx = (uint16_t) chip8.V[x];
            uint16_t vy = (uint16_t) chip8.V[y];
            uint8_t carry = (vx + vy) > 0xFF;
            // store vx + vy in vx
            chip8.V[x] += chip8.V[y];
            // store carry
            chip8.V[0xF] = carry;
        }
        else if (n == 0x5) {
            uint8_t borrow = (chip8.V[x] >= chip8.V[y]); 
            // No borrow if Vx is greater than or equal to Vy
            chip8.V[x] -= chip8.V[y];
            chip8.V[0xF] = borrow; // Store the borrow flag in VF
        }
        else if (n == 0x6) {
            // store lsb in vf
            chip8.V[0xF] = chip8.V[x] & 0x01;
            // shift right by one
            chip8.V[x] >>= 1;
        }
        else if (n == 0x7) {
            uint8_t borrow = (chip8.V[y] >= chip8.V[x]); 
            // No borrow if Vy is greater than or equal to Vx
            chip8.V[x] = chip8.V[y] - chip8.V[x];
            chip8.V[0xF] = borrow; // Store the borrow flag in VF
        }
        else if (n == 0xE) {
            // store msb in vf
            chip8.V[0xF] = chip8.V[x] >> 7;
            // shift left by one
            chip8.V[x] <<= 1;
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