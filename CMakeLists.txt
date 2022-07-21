cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chip8)

find_package(OpenGL REQUIRED)
find_library(GLFW glfw3 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw)
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(chip8 src/chip8.c src/main.c src/display.c src/window.c lib/glad/glad.c)
target_link_libraries(chip8 PUBLIC ${OPENGL_LIBRARIES} ${GLFW})
