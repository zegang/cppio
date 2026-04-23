# Copyright 2025 cppio authors. All rights reserved.
# See the AUTHORS file for names of contributors.

cmake_minimum_required(VERSION 3.10)

project(cppio VERSION 0.0.1 LANGUAGES C CXX)

if(NOT CPPIO_NAMESPACE)
  set(CPPIO_NAMESPACE cppio)
endif(NOT CPPIO_NAMESPACE)
if(NOT CPPIO_OUT)
  set(CPPIO_OUT "")
endif(NOT CPPIO_OUT)

# C standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED OFF)
  set(CMAKE_C_EXTENSIONS OFF)
endif(NOT CMAKE_C_STANDARD)

# C++ standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_CXX_STANDARD)
  # This project requires C++17.
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif(NOT CMAKE_CXX_STANDARD)

if (WIN32)
  set(CPPIO_PLATFORM_NAME CPPIO_PLATFORM_WINDOWS)
  # TODO(cmumford): Make UNICODE configurable for Windows.
  add_definitions(-D_UNICODE -DUNICODE)
else (WIN32)
  set(CPPIO_PLATFORM_NAME CPPIO_PLATFORM_POSIX)
endif (WIN32)

option(CPPIO_BUILD_TESTS "Build CppIO's unit tests" ON)
option(CPPIO_BUILD_BENCHMARKS "Build CppIO's benchmarks" ON)
option(CPPIO_INSTALL "Install CppIO's header and library" ON)

# Print out the build root directory
message("Build root directory: ${CMAKE_BINARY_DIR}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -gdwarf-2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# Specify the path to libstdc++.so
# set(CMAKE_CXX_STANDARD_LIBRARIES "/usr/local/gcc-14/lib64/libstdc++.so")
# Specify custom library search paths
# link_directories(
#     /usr/local/gcc-14/lib64
#     /usr/local/gcc-14/lib
# )
# Specify linker flags to link against specific system libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -lgcc_s -lc -lrt -lpthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--build-id=sha1")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--build-id=sha1")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--build-id=sha1")

set(THIRD_PARTY_INSTALL_ROOT "${CMAKE_BINARY_DIR}/../third_party")
include_directories(${THIRD_PARTY_INSTALL_ROOT})
set(CMAKE_PREFIX_PATH ${THIRD_PARTY_INSTALL_ROOT} )

# Find spdlog package
find_package(spdlog REQUIRED PATHS "${THIRD_PARTY_INSTALL_ROOT}/spdlog")

# Boost
find_package(Boost CONFIG REQUIRED COMPONENTS program_options url
             PATHS "${THIRD_PARTY_INSTALL_ROOT}/boost")
# message(STATUS "Boost_VERSION: ${Boost_VERSION}")
# message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
# message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
# message(STATUS "Boost_PROGRAM_OPTIONS_FOUND: ${Boost_PROGRAM_OPTIONS_FOUND}")
# include_directories(${Boost_INCLUDE_DIRS})

# Find RocksDB package
find_package(RocksDB REQUIRED PATHS "${THIRD_PARTY_INSTALL_ROOT}/rocksdb")

# Minio-CPP
# find_package(miniocpp REQUIRED PATHS "${THIRD_PARTY_INSTALL_ROOT}/minio-cpp")

# aws-sdk-cpp
find_package(aws-sdk-cpp REQUIRED PATHS "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp")
# list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp")
# list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp/lib")
# list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp/lib/cmake")
# find_package(aws-sdk-cpp REQUIRED)
# set(aws-sdk-cpp_INCLUDE_DIRS "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp/include")
# include_directories(${aws-sdk-cpp_INCLUDE_DIRS})
# message(STATUS "aws-sdk-cpp_INCLUDE_DIRS: ${aws-sdk-cpp_INCLUDE_DIRS}")

# nuraft
find_package(NuRaft REQUIRED PATHS "${THIRD_PARTY_INSTALL_ROOT}/nuraft")

# inja
include_directories(third_party/inja)

include_directories(src)
add_subdirectory(src)

set_target_properties(hsd PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN/../lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)
set_target_properties(cli PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN/../lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/cppio_install_root")
# file(GLOB_RECURSE INSTED_3PARTY_LIBS "${THIRD_PARTY_INSTALL_ROOT}/*.so" "${THIRD_PARTY_INSTALL_ROOT}/*.so.*")
# install(FILES ${INSTED_3PARTY_LIBS} DESTINATION ${CPPIO_CMAKE_INSTALL_PREFIX}/lib)
set(THIRD_PRATY_LIBRARY_DIRS
    "${THIRD_PARTY_INSTALL_ROOT}/boost/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/spdlog/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/gflags/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/grpc/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/rocksdb/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/aws-sdk-cpp/lib"
    "${THIRD_PARTY_INSTALL_ROOT}/nuraft/lib"
)
foreach(lib_dir IN LISTS THIRD_PRATY_LIBRARY_DIRS)
    if(EXISTS "${lib_dir}")
        install(DIRECTORY "${lib_dir}/"
                DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
                FILES_MATCHING PATTERN "*.so"
        )
        install(DIRECTORY "${lib_dir}/"
                DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
                FILES_MATCHING PATTERN "*.so.*"
        )
    endif()
endforeach()
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/config DESTINATION ${CMAKE_INSTALL_PREFIX}/)
install(TARGETS hsd cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

set(CPACK_GENERATOR "DEB")
set(CPACK_RPM_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cppio")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILES_INCLUDE_FILES "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RPM_NEED_RELOCATABLE 1)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Zegang Luo <zegang.luo@gmail.com>")

include(CPack)
