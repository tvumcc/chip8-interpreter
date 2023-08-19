#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "chip8.h"
#include "window.h"
#include "opcodes.h"

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

	// Initialize memory to 0
	for (int i = 0; i < 4096; i++) {
		// Load the first 80 bytes with the fontset
		if (i < 80) {
			chip8.memory[i] = fontset[i];
		} else {
			chip8.memory[i] = 0;
		}

	}

	// Load program into memory
	for (int i = 0; i < file_length; i++) {
		chip8.memory[0x200 + i] = buffer[i];
	}
	free(buffer);

	// Initialize registers and stack to 0
	for (int i = 0; i < 16; i++) {
		chip8.registers[i] = 0;
		chip8.stack[i] = 0;
		chip8.keys[i] = 0;
	}

	// Seed the random number generator at the start time of the program
	srand(time(NULL)); 

	// Initialize the rest of the values
	chip8.stack_count = 0;
	chip8.program_counter = 0x200;
	chip8.index_register = 0;
	chip8.delay_timer = 0;
	chip8.sound_timer = 0;
	chip8.program_length = file_length;

	return chip8;
}

void tick(CHIP8* chip8, Display* display, GLFWwindow* window) {
	process_input(window, chip8);
	unsigned short instruction = fetch_instruction(chip8);
	decode_and_execute(window, chip8, display, instruction);
}

unsigned short fetch_instruction(CHIP8* chip8) {
	if (chip8->program_counter <= 0xFFF) {
		unsigned short instruction = (chip8->memory[chip8->program_counter] << 8) | chip8->memory[chip8->program_counter + 1];
		chip8->program_counter += 2;
		return instruction;
	} else {
		return 0x200;
	}
}

void decode_and_execute(GLFWwindow* window, CHIP8* chip8, Display* display, unsigned short instruction) {
	// Possible Arguments
	unsigned char  X   = (instruction & 0x0F00) >> 8;
	unsigned char  Y   = (instruction & 0x00F0) >> 4;
	unsigned char  N   = (instruction & 0x000F);
	unsigned char  NN  = (instruction & 0x00FF);
	unsigned short NNN = (instruction & 0x0FFF);
		
	switch((instruction & 0xF000) >> 12) {
		case 0x00:
	    	if (instruction == 0x00E0) { 
				op_00E0(chip8, display, window);
			} else if (instruction == 0x00EE) { 
				op_00EE(chip8);
			} else {
				printf("Invalid opcode at %04x: %04x\n", chip8->program_counter - 2, instruction);
				exit(EXIT_FAILURE);
			}
			break;
		case 0x01:
			op_1NNN(chip8, NNN);
			break;
		case 0x02:
			op_2NNN(chip8, NNN);
			break;
		case 0x03:
			op_3XNN(chip8, X, NN);
			break;
		case 0x04:
			op_4XNN(chip8, X, NN);
			break;
		case 0x05:
			op_5XY0(chip8, X, Y);
			break;
		case 0x06: 
			op_6XNN(chip8, X, NN);
			break;
		case 0x07: 
			op_7XNN(chip8, X, NN);
			break;
		case 0x08:
		   	switch(N) {
				case 0: 
					op_8XY0(chip8, X, Y);
					break;
				case 1:
					op_8XY1(chip8, X, Y);
					break;
				case 2:
					op_8XY2(chip8, X, Y);
					break;
				case 3:
					op_8XY3(chip8, X, Y);
					break;
				case 4:
					op_8XY4(chip8, X, Y);
					break;
				case 5:
					op_8XY5(chip8, X, Y);
					break;
				case 6:
					op_8XY6(chip8, X);
					break;
				case 7:
					op_8XY7(chip8, X, Y);
					break;
				case 14:
					op_8XYE(chip8, X);
					break;
				default:
					printf("Invalid opcode at %04x: %04x\n", chip8->program_counter - 2, instruction);
					exit(EXIT_FAILURE);
					break;
		   	}
			break;
		case 0x09:
			op_9XY0(chip8, X, Y);
			break;
		case 0x0a:
			op_ANNN(chip8, NNN);
			break;
		case 0x0b:
			op_BNNN(chip8, NNN);	
			break;
		case 0x0c:
			op_CXNN(chip8, X, NN);
			break;	
		case 0x0d:
			op_DXYN(chip8, display, window, X, Y, N);
			break;
		case 0x0e:
			switch(NN) {
				case 0x9e:
					op_EX9E(chip8, X);
					break;
				case 0xa1:
					op_EXA1(chip8, X);
					break;
				default:
					printf("Invalid opcode at %04x: %04x\n", chip8->program_counter - 2, instruction);
					exit(EXIT_FAILURE);
					break;
			}
			break;
		case 0x0f:
			switch(NN) {
				case 0x07:
					op_FX07(chip8, X);
					break;
				case 0x0a:
					op_FX0A(chip8, X);
					break;
				case 0x15:
					op_FX15(chip8, X);
					break;
				case 0x18:
					op_FX18(chip8, X);
					break;
				case 0x1e:
					op_FX1E(chip8, X);
					break;
				case 0x29:
					op_FX29(chip8, X);
					break;
				case 0x33:
					op_FX33(chip8, X);
					break;
				case 0x55:
					op_FX55(chip8, X);
					break;
				case 0x65:
					op_FX65(chip8, X);
					break;
				default:
					printf("Invalide opcode at %04x: %04x\n", chip8->program_counter - 2, instruction);
					exit(EXIT_FAILURE);
					break;
			}
			break;
		default:
			printf("Invalid opcode at %04x: %04x\n", chip8->program_counter - 2, instruction);
			exit(EXIT_FAILURE);
			break;
	}
}
