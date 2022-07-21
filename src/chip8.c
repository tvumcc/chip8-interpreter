#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "chip8.h"

CHIP8 chip8_init(const char* program_path) {
	CHIP8 chip8;
	unsigned char fontset[80] = {
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
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	// Load program from file
	FILE* program_file;
	fopen_s(&program_file, program_path, "rb");
	if (program_file == NULL) {
		perror("Could not open ROM");
		printf("Help: Make sure that the executable is in the same directory as the 'roms' folder");
		exit(EXIT_FAILURE);
	}

	fseek(program_file, 0, SEEK_END);
	int file_length = ftell(program_file);
	rewind(program_file);
	
	unsigned char* buffer = (unsigned char*)(malloc(file_length * sizeof(unsigned char)));
	fread(buffer, file_length, 1, program_file);
	fclose(program_file);


	for (int i = 0; i < 4096; i++) {
		// Load the first 80 bytes with the fontset
		if (i < 80) {
			chip8.memory[i] = fontset[i];
		} else {
			chip8.memory[i] = 0;
		}

	}
	for (int i = 0; i < file_length; i++) {
		chip8.memory[0x1FF + i] = buffer[i];
	}

	free(buffer);
	for (int i = 0; i < 16; i++) {
		chip8.registers[i] = 0;
		chip8.stack[i] = 0;
	}

	chip8.stack_pointer = 0;
	chip8.program_counter = 0x200;
	chip8.index_register = 0;
	chip8.delay_timer = 0;
	chip8.sound_timer = 0;
	chip8.program_length = file_length;

	return chip8;
}

void disassemble(CHIP8* chip8) {
	printf("ADDR ISTR   NME     ARG\n-------------------------\n");
	for (unsigned int i = 0; i < chip8->program_length; i+=2) {
		printf("%04x %02x %02x", chip8->program_counter, chip8->memory[0x1FF + i], chip8->memory[0x1FF + i + 1]);

		unsigned short full_instruction = (chip8->memory[0x1FF + i] << 4) | chip8->memory[0x1FF + i + 1];
		unsigned char first_nibble = chip8->memory[0x1FF + i] >> 4;
		unsigned char second_nibble = chip8->memory[0x1FF + i] & 0x0F;
		unsigned char third_nibble = chip8->memory[0x1FF + i + 1] >> 4;
		unsigned char fourth_nibble = chip8->memory[0x1FF + i + 1] & 0x0F;

		switch(first_nibble) {
			case 0x00:
				if (full_instruction == 0x00E0)
					printf(" CLS\n");
				else
					printf(" ----\n");
				break;
			case 0x01:
				printf(" JUMP    PC, $%x%x%x\n", second_nibble, third_nibble, fourth_nibble);
				break;
			case 0x06:
				printf(" SETR    r%x, #%x%x\n", second_nibble, third_nibble, fourth_nibble);
				break;
			case 0x07:
				printf(" ADD     r%x, #%x%x\n", second_nibble, third_nibble, fourth_nibble);
				break;
			case 0x0a:
				printf(" SETI    I, $%x%x%x \n", second_nibble, third_nibble, fourth_nibble);
				break;
			case 0x0d:
				printf(" DRAW    x(%x), y(%x), h(#%x)\n", chip8->registers[second_nibble], chip8->registers[third_nibble], fourth_nibble);
				break;
			default:
				printf(" ----\n");
				break;
		}
		chip8->program_counter += 2;
	}
	chip8->program_counter = 0x1FF;
}

unsigned short fetch_instruction(CHIP8* chip8) {
	if (chip8->program_counter <= 0xFFF) {
		
		unsigned short instruction = (chip8->memory[chip8->program_counter] << 8) | chip8->memory[chip8->program_counter + 1];
		//printf("%04x\n", instruction);
		chip8->program_counter += 2;
		return instruction;
	} else {
		return 0x200;
	}
}

void decode_and_execute(GLFWwindow* window, CHIP8* chip8, Display* display, unsigned short instruction) {
	// Possible Parameters
	unsigned char  X   = (instruction & 0x0F00) >> 8;
	unsigned char  Y   = (instruction & 0x00F0) >> 4;
	unsigned char  N   = (instruction & 0x000F);
	unsigned char  NN  = (instruction & 0x00FF);
	unsigned short NNN = (instruction & 0x0FFF);

	

	switch((instruction & 0xF000) >> 12) {
		case 0x00: { // Clear (00E0)
			if (instruction == 0x00E0)
				display_clear(display);
				display_texture_update(display);
				display_draw(window, display);
				printf("Screen cleared!\n");
		} break;
		case 0x01: { // Jump (1NNN)
			chip8->program_counter = NNN - 1;
			printf("Jumping to %04x!\n", chip8->program_counter);
			printf("PC: %04x\n", chip8->program_counter);
		} break;
		case 0x06: { // Set Register (6XNN)
			chip8->registers[X] = NN;
			printf("Set value in register %02x to %02x\n", X, NN);
		} break;
		case 0x07: { // Add value to register (7XNN)
			chip8->registers[X] += NN;
			printf("Added value %02x to register %02x\n", NN, X);
		} break;
		case 0x0a: { // Set Index Register (ANNN)
			chip8->index_register = NNN;
			printf("Set index register to value %04x\n", NNN);
		} break;
		case 0x0d: { // Draw (DXYN)
			// Wrap position
			int x_pos = chip8->registers[X] % 64;
			int y_pos = chip8->registers[Y] % 32;
			chip8->registers[15] = 0;
			
			// Set pixels in pixel array
			for (int i = 0; i < N; i++) {
				unsigned char sprite_byte = chip8->memory[chip8->index_register + i - 1];
				int x_pos = chip8->registers[X] % 64;
				for (int j = 0; j < 8; j++) {
					if (((sprite_byte & (0x80 >> j)) >> (7 - j)) == 1) 
						if(display_set_pixel(display, x_pos, y_pos))
							chip8->registers[15] = 1;
					x_pos++;
				}
				y_pos++;
			}

			// Update screen
			display_texture_update(display);
			display_draw(window, display);

			printf("Drew sprite starting at (%d, %d) with a height of %02x\n", x_pos, y_pos, N);
		} break;
		default: {
		} break;
	}
}
