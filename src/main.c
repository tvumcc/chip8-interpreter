#include <time.h>

#include "disassembler.h"
#include "window.h"
#include "display.h"
#include "chip8.h"

Display display; 
CHIP8 chip8;

int main() {
    GLFWwindow* window = window_init(CHIP8_WIDTH * 20, CHIP8_HEIGHT * 20, "CHIP-8"); // Window to render graphics to
	display = display_init(); // Contains information to render graphics (e.g. pixel array, texture, shader)
	chip8 = chip8_init("roms/chip8-test-rom.ch8");
	disassemble_chip8_program(chip8.memory, chip8.program_length);

    while (!glfwWindowShouldClose(window)) {
		tick(&chip8, &display, window);
        glfwPollEvents();
    }
    
    display_destroy(&display);
    glfwTerminate();
    return 0;
}


