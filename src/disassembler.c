#include "disassembler.h"

void view_memory(uint8_t* memory) {
	printf("FONTSET & INTERPRETER");
	for (int i = 0; i < 4096; i++) {
		if (i % 16 == 0) {
			if (i == 0x200) {
				printf("\nPROGRAM STARTS HERE");
			}
			printf("\n%#03x | ", i);
		}
		
		printf("%02x ", memory[i]);
	}
}

void disassemble_chip8_program(uint8_t* memory, unsigned int program_length) {
	unsigned short program_counter;
	printf("ADDRESS  INST    MNEMONIC & OPERANDS  \n");
	printf("-------+------+----------------------+\n");
	for (program_counter = 0x200; program_counter < program_length + 0x200; program_counter+=2) {
		printf(" %#04X | %02X%02X | ", program_counter, memory[program_counter], memory[program_counter + 1]);
		switch((memory[program_counter] & 0xF0) >> 4) {
			case 0x00:
				if (memory[program_counter + 1] == 0xE0)
					printf("CLS ; Clear display");
				if (memory[program_counter + 1] == 0xEE)
					printf("RET ; Return from subroutine");
				break;
			case 0x01:
				printf("JMP  $#%02x%02x ; Jump to address", memory[program_counter] >> 4, memory[program_counter + 1]);
				break;
			case 0x02:
				printf("CAL  $#%02x%02x ; Call subroutine at address", memory[program_counter] >> 4, memory[program_counter + 1]);
				break;
			case 0x03:
				printf("SEQ  V%02x, #%02x ; Skip if VX == NN", memory[program_counter] & 0x0F, memory[program_counter + 1]);
				break;
			case 0x04:
				printf("SNE  V%02x, #%02x ; Skip if VX != NN", memory[program_counter] & 0x0F, memory[program_counter + 1]);
				break;
			case 0x05:
				printf("SRE  V%02x, V%02x ; Skip if VX == NN", memory[program_counter] & 0x0F, memory[program_counter + 1] & 0x0F);
				break;
			case 0x06:
				printf("LOR  V%02x, #%02x ; Load VX with NN", memory[program_counter] & 0x0F, memory[program_counter + 1]);
				break;
			case 0x07:
				printf("ADD  V%02x, #%02x ; Add NN to VX (VX += NN)", memory[program_counter] & 0x0F, memory[program_counter + 1]);
				break;
			case 0x08:
				switch(memory[program_counter + 1] & 0x0F) {
					case 0x00:
						printf("MOV  V%02x, V%02x ; Move VY into VX (VX = VY)", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x01:
						printf("BOR  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x02:
						printf("AND  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x03:
						printf("XOR  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x04:
						printf("ADR  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x05:
						printf("SUB  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x06:
						printf("SHR  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x07:
						printf("SBR  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					case 0x0E:
						printf("SHL  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
						break;
					default:
						break;
				}
				break;
			case 0x09:
				printf("SRN  V%02x, V%02x ", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4);
				break;
			case 0x0A:
				printf("LOI  #%02x%02x", memory[program_counter] >> 4, memory[program_counter + 1]);
				break;
			case 0x0B:
				printf("JVR  #%02x%02x", memory[program_counter] >> 4, memory[program_counter + 1]);
				break;
			case 0x0C:
				printf("RAN  V%02x, #%02x", memory[program_counter] & 0x0F, memory[program_counter + 1]);
				break;
			case 0x0D:
				printf("DRW  V%02x, V%02x, %02x", memory[program_counter] & 0x0F, (memory[program_counter + 1] & 0xF0) >> 4, memory[program_counter + 1] & 0x0F);
				break;
			case 0x0E:
				switch(memory[program_counter + 1]) {
					case 0x9E:
						printf("KEE  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0xA1:
						printf("KEN  V%02x", memory[program_counter] & 0x0F);
						break;
					default:
						break;
				}
			case 0x0F:
				switch(memory[program_counter + 1]) {
					case 0x07:
						printf("SRT  V%02x", memory[program_counter] & 0x0F); 
						break;
					case 0x0A:
						printf("WFK  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x15:
						printf("SDT  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x18:
						printf("SST  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x1E:
						printf("ARI  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x29:
						printf("SIF  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x33:
						printf("BCD  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x55:
						printf("WRT  V%02x", memory[program_counter] & 0x0F);
						break;
					case 0x65:
						printf("LOA  V%02x", memory[program_counter] & 0x0F);
						break;
					default:
						break;
				}
			default:
				break;
		}
		printf("\n");
	}
}
