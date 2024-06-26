#!/bin/bash

# Specify Boost version and installation directory
BOOST_VERSION="1.78.0"
INSTALL_DIR="deps/build.out/"

# Download Boost source
wget https://boostorg.jfrog.io/artifactory/main/release/${BOOST_VERSION}/source/boost_${BOOST_VERSION}.tar.gz

# Extract Boost source
tar -xzf boost_${BOOST_VERSION}.tar.gz
cd boost_${BOOST_VERSION}

# Bootstrap Boost.Build
./bootstrap.sh --prefix=${INSTALL_DIR}

# Build Boost libraries (adjust --with options as needed)
./b2 --prefix=${INSTALL_DIR} --with-program_options --with-system

# Install Boost libraries
./b2 install

# Clean up: Remove the downloaded archive and extracted directory
cd ..
rm -rf boost_${BOOST_VERSION} boost_${BOOST_VERSION}.tar.gz