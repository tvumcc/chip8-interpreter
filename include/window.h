#ifndef WINDOW_H
#define WINDOW_H
#include "glad/glad.h"
#include "glfw/glfw3.h"

#include "chip8.h"

// Create a window and context
GLFWwindow* window_init(int width, int height, const char* caption);
void process_input(GLFWwindow* window, CHIP8* chip8);
#endif
