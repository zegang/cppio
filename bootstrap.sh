#!/bin/bash
echo "-------------------------------------"
echo "To run cppio bootstrap.sh"
echo "-------------------------------------"

ROOT_DIR=`pwd`
CXXSTD="c++20"
CC=/usr/local/gcc-14/bin/gcc
CXX=/usr/local/gcc-14/bin/g++
CXXFLAGS="-O2 -g -std=${CXXSTD}"  # Set C++ standard as needed
LDFLAGS="-L/usr/local/gcc-14/lib64"  # Path to custom libstdc++.so
CXX_STANDARD_LIBRARIES="/usr/local/gcc-14/lib64/libstdc++.so"
LD_LIBRARY_PATH=/usr/local/gcc-14/lib64:/usr/local/gcc-14/lib:$LD_LIBRARY_PATH

PATH=/usr/local/gcc-14/bin:/usr/local/gcc-14/lib64:/usr/local/gcc-14/lib:$PATH


echo -e "ROOT_DIR ${ROOT_DIR}\n"

############# Boost ##############
cd ${ROOT_DIR}
BOOST_SRC="${ROOT_DIR}/deps/boost"
BOOST_INSTALL_DIR="${ROOT_DIR}/build/deps/boost"
rm -rf ${BOOST_INSTALL_DIR}
cd ${BOOST_SRC}
# Bootstrap Boost.Build
./bootstrap.sh --prefix=${BOOST_INSTALL_DIR}
# Build Boost libraries (adjust --with options as needed)
./b2 --prefix=${BOOST_INSTALL_DIR} --with-program_options --with-beast --with-asio toolset=gcc-14 cxxflags=${CXXFLAGS}
# ./b2 --prefix=${INSTALL_DIR} --with-program_options link=static runtime-link=static
# Install Boost libraries
./b2 install

############# spdlog ##############
cd ${ROOT_DIR}
SPDLOG_ROOT="${ROOT_DIR}/deps/spdlog"
cd ${SPDLOG_ROOT}
mkdir build
cmake -S . -B build -DCMAKE_INSTALL_PREFIX="${ROOT_DIR}/build/deps/spdlog"
cd build
make install

############# Back to Root DIR ##############
cd ${ROOT_DIR}

echo "-------------------------------------"
echo "Finish cppio bootstrap.sh"
echo "-------------------------------------"