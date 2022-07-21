#ifndef DISPLAY_H
#define DISPLAY_H
#include "glad/glad.h"
#include "glfw/glfw3.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

typedef struct Display {
    unsigned char* pixelArray;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int texture;
    unsigned int shaderProgram;
} Display;

// Initialize the display array and texture
Display display_init();

// Uninitialize a Display
void display_destroy(Display* display);

// Draw the display texture to the window
void display_draw(GLFWwindow* window, Display* display);

// Update the display texture from the display array
void display_texture_update(Display* display);

// Set pixel of display array
bool display_set_pixel(Display* display, int x, int y);

// Set all pixels of display array to 0x00 (black)
void display_clear(Display* display);
#endif
