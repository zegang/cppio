# Copyright 2025 cppio authors. All rights reserved.
# See the AUTHORS file for names of contributors.

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(GLOB_RECURSE UTIL_SOURCES *.cc)

add_library(util ${UTIL_SOURCES})

target_compile_definitions(util PRIVATE CPPIO_NAMESPACE=${CPPIO_NAMESPACE} CPPIO_OUT=${CPPIO_OUT})

# target_include_directories(cli
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/../include ALIAS my_includes)

message(STATUS "CPPIO_INCLUDE_DIR: ${CPPIO_INCLUDE_DIR}")
target_include_directories(util PRIVATE ${CPPIO_INCLUDE_DIR})