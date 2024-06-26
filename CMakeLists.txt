cmake_minimum_required(VERSION 3.0)  # CMake version check
project(bars_generator)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 23)            

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.cpp 
    bars_generator.hpp
    bars_generator.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(main main.cpp
    bars_generator.hpp
    bars_generator.cpp)