#include "window.h"
#include "display.h"
#include "chip8.h"

Display display; // Contains information to render graphics (e.g. pixel array, texture, shader)
CHIP8 chip8;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
    GLFWwindow* window = windowInit(CHIP8_WIDTH * 20, CHIP8_HEIGHT * 20, "CHIP-8"); // Window to render graphics to
	display = display_init(); // Contains information to render graphics (e.g. pixel array, texture, shader)
	chip8 = chip8_init("roms/ibm_logo.ch8");

	glfwSetKeyCallback(window, key_callback);
	
	disassemble(&chip8);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    display_destroy(&display);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		unsigned short instruction = fetch_instruction(&chip8);
		printf("W key pressed; executing instruction: %04x\n", instruction);
		decode_and_execute(window, &chip8, &display, instruction);
	}
}

