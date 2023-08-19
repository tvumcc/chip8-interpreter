#include "display.h"

Display display_init() {
    // Initialize pixelArray
    unsigned char* pixelArray = (unsigned char*)(malloc((CHIP8_WIDTH * CHIP8_HEIGHT)));
    for (int i = 0; i < (CHIP8_WIDTH * CHIP8_HEIGHT); i++) {
        pixelArray[i] = 0x00;
    }

    unsigned int texture;
    unsigned int VAO, VBO, EBO;
    unsigned int shaderProgram;
    
    // Texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, CHIP8_WIDTH, CHIP8_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, pixelArray);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // VBO
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    unsigned int indices[] = {
        0, 2, 1,
        0, 3, 2,
    };

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Vertex Shader
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoords;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   TexCoords = aTexCoord;\n"
        "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoords;\n"
        "uniform sampler2D displayTexture;\n"
        "void main() {\n"
        "   if(texture(displayTexture, TexCoords).r == 1) {\n"
        "       FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "   } else {\n"
        "       FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "   }\n"
        "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program
    shaderProgram  = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader); glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "displayTexture"), 0);
    glUseProgram(0);

    Display display;
    display.pixelArray = pixelArray;
    display.texture = texture;
    display.VBO = VBO;
    display.EBO = EBO;
    display.VAO = VAO;
    display.shaderProgram = shaderProgram;
    return display;
}

void display_destroy(Display* display) {
    glDeleteVertexArrays(1, &display->VAO);
    glDeleteBuffers(1, &display->VBO);
    glDeleteBuffers(1, &display->EBO);
    glDeleteProgram(display->shaderProgram);
    free(display->pixelArray);
}

void display_draw(GLFWwindow* window, Display* display) {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(display->shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, display->texture);
    glBindVertexArray(display->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
}   

void display_texture_update(Display* display) {
    glBindTexture(GL_TEXTURE_2D, display->texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CHIP8_WIDTH, CHIP8_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, display->pixelArray);
}

bool display_set_pixel(Display* display, int x, int y) {
    // if ((x < CHIP8_WIDTH && x >= 0) && (y >= 0 && y < CHIP8_HEIGHT)) {
        if (display->pixelArray[64 * y + x] == 0xFF) {
            //display->pixelArray[64 * y + x] = 0x00;
            return false; // Flipped
        } else {
            display->pixelArray[64 * y + x] = 0xFF;
            return false; // Did not flip
        }
    // } else {
    //     return false;
    // }
}

void display_clear(Display* display) {
    for (int i = 0; i < (CHIP8_WIDTH * CHIP8_HEIGHT); i++) {
        display->pixelArray[i] = 0x00;
    }
}
