
# CHIP-8 Interpreter

Check out this [link](https://en.wikipedia.org/wiki/CHIP-8) to learn more about CHIP-8.

## Building

To build the project, first clone the repository recursively (to fetch submodules):

```bash
git clone https://github.com/piigle/chip8-interpreter.git --recursive
```

Then, make a build directory and use CMake to build the project:

```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build .
```

If you are having problems loading the ROMs, make sure to run the executable from the project directory (the same directory in which the `roms` folder is contained).
