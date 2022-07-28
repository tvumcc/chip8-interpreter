#include <stdint.h>
#include <stdio.h>

#include "window.h"
#include "display.h"
#include "opcodes.h"
#include "chip8.h"

void op_00E0(CHIP8* chip8, Display* display, GLFWwindow* window) {
	display_clear(display);	
	display_texture_update(display);
	display_draw(window, display);

#ifdef CHIP8_DEBUG
	printf("$%04x: Screen cleared\n", chip8->program_counter);
#endif
}

void op_00EE(CHIP8* chip8) {
	if (chip8->stack_count <= 0 || chip8->stack_count > 16) {
		printf("00EE at address %04x: Error returning from subroutine there is no subroutine to return from!", chip8->program_counter);
		exit(EXIT_FAILURE);
	}
	unsigned short return_address = chip8->stack[chip8->stack_count - 1];
	chip8->program_counter = return_address;
	chip8->stack[chip8->stack_count - 1] = 0;
	chip8->stack_count--;
}

void op_1NNN(CHIP8* chip8, uint16_t NNN) {
#ifdef CHIP8_DEBUG
	printf("$%04x: Jumped to $%04x\n", chip8->program_counter, NNN);
#endif
	chip8->program_counter = NNN;
}

void op_2NNN(CHIP8* chip8, uint16_t NNN) {
	if (chip8->stack_count > 16) {
		printf("2NNN at address %04x: Error adding subroutine to stack, stack overflow\n", chip8->program_counter);
		exit(EXIT_FAILURE);
	}
	// Push the current program counter onto the stack so that it can be returned to later
	chip8->stack[chip8->stack_count] = chip8->program_counter;
	chip8->stack_count++;
	// Jump to the memory address
	chip8->program_counter = NNN;

#ifdef CHIP8_DEBUG
	printf("Calling subroutine at address %04x\n", chip8->program_counter);
#endif
}

void op_3XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	if (chip8->registers[X] == NN) 
		chip8->program_counter += 2;

#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x(%d) == NN(%d), Skipping next instruction\n", chip8->program_counter, X, chip8->registers[X], NN);
#endif
}

void op_4XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	if (chip8->registers[X] != NN)
		chip8->program_counter += 2;

#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x(%d) != NN(%d), Skipping next instruction\n", chip8->program_counter, X, chip8->registers[X], NN);
#endif
}

void op_5XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	if (chip8->registers[X] == chip8->registers[Y])
		chip8->program_counter += 2;

#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x(%d) == V%02x(%d), Skipping next instruction\n", chip8->program_counter, X, chip8->registers[X], Y, chip8->registers[Y]);
#endif
}

void op_6XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] = NN;

#ifdef CHIP8_DEBUG
	printf("$%04x: Set V%02x to %d\n", chip8->program_counter, X, NN);
#endif
}

void op_7XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] += NN;

#ifdef CHIP8_DEBUG
	printf("$%04x: Added value %d to V%02x\n", chip8->program_counter, NN, X);
#endif
}

void op_8XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] = chip8->registers[Y];

#ifdef CHIP8_DEBUG
	printf("$%04x: Set V%02x to V%02x\n", chip8->program_counter, X, Y);
#endif
}

void op_8XY1(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] |= chip8->registers[Y];

#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x |= V%02x\n", chip8->program_counter, X, Y);
#endif
}

void op_8XY2(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] &= chip8->registers[Y];
	
#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x &= V%02x\n", chip8->program_counter, X, Y);
#endif
}

void op_8XY3(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] ^= chip8->registers[Y];

#ifdef CHIP8_DEBUG
	printf("$%04x: V%02x ^= V%02x\n", chip8->program_counter, X, Y);
#endif
}

void op_8XY4(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[X] + chip8->registers[Y];
	if (result > 255) {
		chip8->registers[15] = 1;
		result = (result % 255) - 1;
	} else {
		chip8->registers[15] = 0;
	}

#ifdef CHIP8_DEBUG
	printf("%04x: V%02x(%d) + V%02x(%d) = %d; VF(%d)\n", chip8->program_counter, X, chip8->registers[X], Y, chip8->registers[Y], result, chip8->registers[15]);
#endif
	chip8->registers[X] = (uint8_t)(result);
}

void op_8XY5(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[X] - chip8->registers[Y];
	chip8->registers[15] = 1;

	if (chip8->registers[X] < chip8->registers[Y]) {
		result = 256 + result;
		chip8->registers[15] = 0;
	}
#ifdef CHIP8_DEBUG
	printf("%04x: V%02x(%d) - V%02x(%d) = %d; VF(%d)\n", chip8->program_counter, X, chip8->registers[X], Y, chip8->registers[Y], result, chip8->registers[15]);
#endif
	chip8->registers[X] = (uint8_t)(result);
}

void op_8XY6(CHIP8* chip8, uint8_t X) {
	uint8_t lsb = chip8->registers[X] & 0x01;
	chip8->registers[X] >>= 1;
	chip8->registers[15] = lsb == 1 ? 1 : 0;	
}

void op_8XY7(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[Y] - chip8->registers[X];
	chip8->registers[15] = 1;
	if (chip8->registers[Y] < chip8->registers[X]) {
		result = 256 + result;
		chip8->registers[15] = 0;
	}

	chip8->registers[X] = (uint8_t)(result);
}

void op_8XYE(CHIP8* chip8, uint8_t X) {
	uint8_t msb = (chip8->registers[X] & 0x80) >> 7; 
	chip8->registers[X] <<= 1;
	chip8->registers[15] = msb == 1 ? 1 : 0;

#ifdef CHIP8_DEBUG
	printf("%04x: V%02x >>= 1; VF(%d)\n", chip8->program_counter, X, chip8->registers[15]);
#endif
}

void op_9XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	if (chip8->registers[X] != chip8->registers[Y])
		chip8->program_counter += 2;

#ifdef CHIP8_DEBUG
	printf("%04x: V%02x != V%02x; Skipping next instruction\n", chip8->program_counter, X, Y);
#endif
}

void op_ANNN(CHIP8* chip8, uint16_t NNN) {
	chip8->index_register = NNN;

#ifdef CHIP8_DEBUG
	printf("%04x: Set index register to value %04x\n", chip8->program_counter, NNN);
#endif
}

void op_BNNN(CHIP8* chip8, uint16_t NNN) {
#ifdef CHIP8_DEBUG
	printf("%04x: Jumping to (%04x + V0(%02x)) = %04x\n", chip8->program_counter, NNN, chip8->registers[0], ((NNN + chip8->registers[0]) - 1));
#endif
	chip8->program_counter = (NNN + chip8->registers[0]) - 1;
}

void op_CXNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] = (uint8_t)((rand() % 256) - 1) & NN;
#ifdef CHIP8_DEBUG
	printf("%04x: Random number generated: %d\n", chip8->program_counter, chip8->registers[X]);
#endif
}

void op_DXYN(CHIP8* chip8, Display* display, GLFWwindow* window, uint8_t X, uint8_t Y, uint8_t N) {
	chip8->registers[15] = 0;

	int y_pos = chip8->registers[Y] % 32;
	for (uint8_t i = 0; i < N; i++) {
		int x_pos = chip8->registers[X] % 64;
		uint8_t sprite_byte = chip8->memory[chip8->index_register + i];
		for (int j = 0; j < 8; j++) {
			if (((sprite_byte & (0x80 >> j)) >> (7 - j)) == 1) // Check if the bit needed is set to 1, if so draw it
				if (display_set_pixel(display, x_pos, y_pos))
					chip8->registers[15] = 1;
			x_pos++;
		}
		y_pos++;
	}
	display_texture_update(display);
	display_draw(window, display);

#ifdef CHIP8_DEBUG
	printf("%04x: Drew sprite starting at (%d, %d) with a height of %d\n", chip8->program_counter, chip8->registers[X], chip8->registers[Y], N);
#endif
}

void op_EX9E(CHIP8* chip8, uint8_t X) {
	if (chip8->keys[chip8->registers[X]] == 1) {
		printf("Key detected, skipping\n");
		chip8->program_counter += 2;
	}
}

void op_EXA1(CHIP8* chip8, uint8_t X) {
	if (chip8->keys[chip8->registers[X]] == 0)
		chip8->program_counter += 2;
}

void op_FX07(CHIP8* chip8, uint8_t X) {
	chip8->registers[X] = chip8->delay_timer;	
}

void op_FX0A(CHIP8* chip8, uint8_t X) {
	for (int i = 0; i < 16; i++) {
		if (chip8->keys[i] == 1) {
			chip8->registers[X] = chip8->keys[i];
			printf("KEY FOUND\n");
			return;
		}	
	}
	chip8->program_counter -= 2;
}

void op_FX15(CHIP8* chip8, uint8_t X) {
	chip8->delay_timer = chip8->registers[X];
}

void op_FX18(CHIP8* chip8, uint8_t X) {
	chip8->sound_timer = chip8->registers[X];
}

void op_FX1E(CHIP8* chip8, uint8_t X) {
	chip8->index_register += chip8->registers[X];	
}

void op_FX29(CHIP8* chip8, uint8_t X) {
	chip8->index_register = X * 5;
}

void op_FX33(CHIP8* chip8, uint8_t X) {
	uint8_t number = chip8->registers[X];
	uint8_t digit1 = number / 100;
	uint8_t digit2 = (number % 100) / 10;
	uint8_t digit3 = number % 10;
	chip8->memory[chip8->index_register] = digit1;
	chip8->memory[chip8->index_register + 1] = digit2;
	chip8->memory[chip8->index_register + 2] = digit3;
}

void op_FX55(CHIP8* chip8, uint8_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		chip8->memory[chip8->index_register + i] = chip8->registers[i];
	}
}

void op_FX65(CHIP8* chip8, uint8_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		chip8->registers[i] = chip8->memory[chip8->index_register + i];
	}
}
