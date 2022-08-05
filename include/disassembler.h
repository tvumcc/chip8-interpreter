#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
#include <stdint.h>

void view_memory(uint8_t* memory);
void disassemble_chip8_program(uint8_t* memory, unsigned int program_length);

#endif