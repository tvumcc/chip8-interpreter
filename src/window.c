#include "window.h"

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* window_init(int width, int height, const char* caption) {
    glfwInit();
    // Use OpenGL version 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(width, height, caption, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    return window;
}

void process_input(GLFWwindow* window, CHIP8* chip8) {
	/* COSMAC VIP Keypad   Computer Keypad Equivalent
	 *		 1 2 3 C           1 2 3 4
	 *		 4 5 6 D           Q W E R
	 *		 7 8 9 E		   A S D F
	 *		 A 0 B F           Z X C V
	 */
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		chip8->keys[0] = 1;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
		chip8->keys[0] = 0;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		chip8->keys[1] = 1;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
		chip8->keys[1] = 0;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		chip8->keys[2] = 1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
		chip8->keys[2] = 0;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		chip8->keys[3] = 1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
		chip8->keys[3] = 0;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		chip8->keys[4] = 1;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
		chip8->keys[4] = 0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		chip8->keys[5] = 1;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
		chip8->keys[5] = 0;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		chip8->keys[6] = 1;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
		chip8->keys[6] = 0;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		chip8->keys[7] = 1;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
		chip8->keys[7] = 0;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		chip8->keys[8] = 1;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
		chip8->keys[8] = 0;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		chip8->keys[9] = 1;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
		chip8->keys[9] = 0;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		chip8->keys[10] = 1;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		chip8->keys[10] = 0;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		chip8->keys[11] = 1;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		chip8->keys[11] = 0;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		chip8->keys[12] = 1;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
		chip8->keys[12] = 0;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		chip8->keys[13] = 1;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		chip8->keys[13] = 0;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		chip8->keys[14] = 1;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
		chip8->keys[14] = 0;

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		chip8->keys[15] = 1;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
		chip8->keys[15] = 0;
}
