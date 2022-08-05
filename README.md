
# CHIP-8 Interpreter

Check out this [link](https://en.wikipedia.org/wiki/CHIP-8) to learn more about CHIP-8.

## Building

If you are building for Linux or a build system other than MSVC then acquire a [GLFW](https://www.glfw.org/download.html) library file to link with.
If you are building for Windows with Visual Studio, there is already a library file in the repository

To build the project, simply use CMake.

```batch
cmake -G "Visual Studio 17 2022"
cmake --build .
```

Replace "Visual Studio 17 2022" with your Cmake generator of choice.
