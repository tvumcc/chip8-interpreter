#include <time.h>

#include "disassembler.h"
#include "window.h"
#include "display.h"
#include "chip8.h"

int main() {
    float lastFrame = glfwGetTime();
	float lastDecrement = glfwGetTime();
    float clock_speed = 0.001f; // 1 for slow, 0.000001 for fast

    GLFWwindow* window = window_init(CHIP8_WIDTH * 20, CHIP8_HEIGHT * 20, "CHIP-8"); // Window to render graphics to
	Display display = display_init(); // Contains information to render graphics (e.g. pixel array, texture, shader)
	CHIP8 chip8 = chip8_init("roms/5-quirks.ch8");
    disassemble_chip8_program(&chip8.memory, chip8.program_length);

    while (!glfwWindowShouldClose(window)) {
		if (glfwGetTime() > lastDecrement + 0.016f) {
			if (chip8.sound_timer > 0)	
				chip8.sound_timer--;
			if (chip8.delay_timer > 0)
				chip8.delay_timer--;

			lastDecrement = glfwGetTime();
		}
		
        if (glfwGetTime() > lastFrame + clock_speed) {
            tick(&chip8, &display, window);
            glfwPollEvents();
            lastFrame = glfwGetTime();
        }
        glfwPollEvents();
    }
    
    display_destroy(&display);
    glfwTerminate();
    return 0;
}


