cmake_minimum_required(VERSION 3.0.0)
project(hello_window VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(hello_window 
    src/config.h 
    src/main.cpp 
    src/glad.c
)

target_include_directories(hello_window PRIVATE dependencies)

target_link_libraries(hello_window glfw OpenGL::GL)