cmake_minimum_required(VERSION 0.1)

project(CppIO)

# Add all your source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create an executable target from sources
add_executable(CppIOExecutable ${SOURCES})
