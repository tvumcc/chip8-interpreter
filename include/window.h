#ifndef WINDOW_H
#define WINDOW_H
#include "glad/glad.h"
#include "glfw/glfw3.h"

// Create a window and context
GLFWwindow* windowInit(int width, int height, const char* caption);
#endif
