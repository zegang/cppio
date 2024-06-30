cmake_minimum_required(VERSION 3.10)

project(CppIO)

set(CMAKE_BUILD_TYPE Debug)

# Print out the build root directory
message("Build root directory: ${CMAKE_BINARY_DIR}")

# Specify the compiler
set(CMAKE_CXX_COMPILER "/usr/local/gcc-14/bin/c++")
# # Static build requires setting appropriate flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
# # Set static linking for all libraries (optional)
# set(CMAKE_EXE_LINKER_FLAGS "-static")
# Set C++ standard to C++17
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set C++ compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")
# Optionally, set debug-specific flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
# Optionally, set release-specific flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
# Specify the path to libstdc++.so
set(CMAKE_CXX_STANDARD_LIBRARIES "/usr/local/gcc-14/lib64/libstdc++.so")
# Specify custom library search paths
link_directories(
    /usr/local/gcc-14/lib64
    /usr/local/gcc-14/lib
)
# Specify linker flags to link against specific system libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -lgcc_s -lc -lrt -lpthread")

set(DPES_ROOT "${CMAKE_BINARY_DIR}/deps")
include_directories(${DPES_ROOT})

# Find spdlog package
find_package(spdlog REQUIRED PATHS "${DPES_ROOT}/spdlog")

# Set paths to Boost headers and libraries
set(BOOST_ROOT "${DPES_ROOT}/boost")
# set(BOOST_INCLUDEDIR "${CMAKE_BINARY_DIR}/deps/boost/include")
# set(BOOST_LIBRARYDIR "${CMAKE_BINARY_DIR}/deps/boost/lib")

# Find Boost
# find_package(Boost REQUIRED COMPONENTS program_options
#              PATHS ${BOOST_INCLUDEDIR} ${BOOST_LIBRARYDIR})
find_package(Boost REQUIRED COMPONENTS program_options)

# Print out Boost information
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_PROGRAM_OPTIONS_FOUND: ${Boost_PROGRAM_OPTIONS_FOUND}")

# Specify additional include directories for Boost headers
include_directories(${Boost_INCLUDE_DIRS})

# Example with explicit compiler flags
# if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     add_compile_options(-std=c++14)
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#     add_compile_options(/std:c++17)
# endif()

# Enable compiler extensions if needed (optional)
# set(CMAKE_CXX_EXTENSIONS ON)
# # Set Boost root directory
# set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/deps/boost)
# # Include Boost's CMake configuration
# add_subdirectory(${BOOST_ROOT})
# # Add Boost include directories
# include_directories(${BOOST_ROOT})

# Add all your source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create an executable target from sources
add_executable(cppio ${SOURCES})

# Include directories for Boost headers
target_include_directories(cppio PRIVATE ${Boost_INCLUDE_DIRS})
# Link Boost libraries
target_link_libraries(cppio PRIVATE ${Boost_LIBRARIES} spdlog::spdlog_header_only)

# Set C++ standard
set_target_properties(cppio PROPERTIES CXX_STANDARD 20)