#!/bin/bash
# cppio-bootstrap.sh
# Usage: ./cppio-bootstrap.sh [target] [sub-target] [--reclone-submodules]
# Example: ./cppio-bootstrap.sh all all --reclone-submodules

set -e

usage() {
    echo "Usage: $0 [target] [sub-target] [--reclone-submodules]"
    echo "Available targets:"
    echo "  all          Build all components"
    echo "  cmake        Build CMake"
    echo "  boost        Build Boost libraries"
    echo "  spdlog       Build spdlog"
    echo "  grpc         Build gRPC"
    echo "  gflags       Build gflags"
    echo "  rocksdb      Build RocksDB"
    echo "  aws-sdk-cpp  Build AWS SDK for C++"
    echo "  minio-cpp    Build MinIO C++ SDK"
    echo "Sub-targets:"
    echo "  all          Build all sub-components"
    echo "  clean        Clean the build"
    echo "  --reclone-submodules  Remove and re-clone all submodules listed in .gitmodules"
}

reclone_submodules() {
    echo "###########################"
    echo "Re-cloning all submodules from .gitmodules"
    echo "###########################"
    if [ ! -f .gitmodules ]; then
        echo ".gitmodules file not found!"
        return 1
    fi
    local paths=()
    local urls=()
    while IFS= read -r line; do
        if [[ $line =~ path\ =\ (.*) ]]; then
            paths+=("${BASH_REMATCH[1]}")
        elif [[ $line =~ url\ =\ (.*) ]]; then
            urls+=("${BASH_REMATCH[1]}")
        fi
    done < .gitmodules
    for i in "${!paths[@]}"; do
        local path="${paths[$i]}"
        local url="${urls[$i]}"
        echo "Removing submodule directory: $path"
        rm -rf "$path"
        echo "Cloning $url to $path"
        git clone "$url" "$path"
    done
    git submodule sync --recursive
    echo "All submodules have been re-cloned from their URLs."
}

RECLONE_SUBMODULES=false
for arg in "$@"; do
    if [ "$arg" == "--reclone-submodules" ]; then
        RECLONE_SUBMODULES=true
    fi
    if [ "$arg" == "help" ] || [ "$arg" == "--help" ] || [ "$arg" == "-h" ]; then
        usage
        exit 0
    fi
}

if [ "$RECLONE_SUBMODULES" = true ]; then
    reclone_submodules
    exit 0
fi

MY_BUILD_TARGET="${1:-all}"
MY_BUILD_SUB_TARGET="${2:-all}"

ROOT_DIR="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
CXXSTD="c++20"
CXXFLAGS="-O2 -g -std=${CXXSTD}"
THIRD_PARTY_DIR="${ROOT_DIR}/third_party"
THIRD_PARTY_BUILD_INSTALL_DIR="${ROOT_DIR}/build/third_party"

update_submodules() {
    echo "###########################"
    echo "Updating Submodules $MY_BUILD_TARGET"
    echo "###########################"
    git submodule sync --recursive
    if [ "$MY_BUILD_TARGET" == "all" ]; then
        git submodule update --init --recursive
    else
        git submodule update --init --recursive third_party/$MY_BUILD_TARGET
    fi
}

if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
    update_submodules
fi

build_cmake() {
    echo "###########################"
    echo "Building CMake"
    echo "###########################"
    cd "$ROOT_DIR"
    CMAKE_SRC="$THIRD_PARTY_DIR/cmake"
    CMAKE_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/cmake"
    rm -rf "$CMAKE_INSTALL_DIR"
    cd "$CMAKE_SRC"
    git checkout v3.31.8
    ./bootstrap
    make
    make install
}

clean_cmake() {
    echo "Cleaning CMake"
    mmake uninstall
}

build_boost() {
    echo "###########################"
    echo "Building Boost"
    echo "###########################"
    cd "$ROOT_DIR"
    BOOST_SRC="$THIRD_PARTY_DIR/boost"
    BOOST_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/boost"
    rm -rf "$BOOST_INSTALL_DIR"
    cd "$BOOST_SRC"
    ./bootstrap.sh --prefix="$BOOST_INSTALL_DIR"
    ./b2 --prefix="$BOOST_INSTALL_DIR" --with-program_options --with-beast --with-asio toolset=gcc-14 cxxflags="$CXXFLAGS"
    ./b2 install
}

clean_boost() {
    echo "Cleaning Boost"
}

build_spdlog() {
    echo "###########################"
    echo "Building spdlog"
    echo "###########################"
    cd "$ROOT_DIR"
    SPDLOG_ROOT="$THIRD_PARTY_DIR/spdlog"
    cd "$SPDLOG_ROOT"
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DCMAKE_INSTALL_PREFIX="$THIRD_PARTY_BUILD_INSTALL_DIR/spdlog"
    pushd cmake/build
    make install
    popd
}

clean_spdlog() {
    echo "Cleaning spdlog"
    rm -rf cmake
}

build_grpc() {
    echo "###########################"
    echo "Building gRPC"
    echo "###########################"
    cd "$ROOT_DIR"
    GRPC_ROOT="$THIRD_PARTY_DIR/grpc"
    GRPC_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/grpc"
    rm -rf "$GRPC_INSTALL_DIR"
    cd "$GRPC_ROOT"
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX="$GRPC_INSTALL_DIR"
    pushd cmake/build
    make -j 4
    make install
    popd
    # Test gRPC helloworld
    cd "$GRPC_ROOT/examples/cpp/helloworld"
    rm -rf cmake/build
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DCMAKE_PREFIX_PATH="$GRPC_INSTALL_DIR/lib/cmake/" -DCMAKE_INSTALL_PREFIX="$GRPC_INSTALL_DIR/examples/cpp/helloworld"
    pushd cmake/build
    make -j 4
    ./greeter_server 2>&1 &
    grpcServerPID=$!
    sleep 5s
    ./greeter_client
    sleep 5s
    kill $grpcServerPID
    popd
}

clean_grpc() {
    echo "Cleaning gRPC"
    cd "$THIRD_PARTY_DIR/grpc"
    rm -rf cmake
}

build_gflags() {
    echo "###########################"
    echo "Building gflags"
    echo "###########################"
    cd "$ROOT_DIR"
    gflags_ROOT="$THIRD_PARTY_DIR/gflags"
    gflags_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/gflags"
    rm -rf "$gflags_INSTALL_DIR"
    cd "$gflags_ROOT"
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DCMAKE_INSTALL_PREFIX="$gflags_INSTALL_DIR" -DBUILD_SHARED_LIBS=ON
    pushd cmake/build
    make -j 4
    make install
    popd
}

clean_gflags() {
    echo "Cleaning gflags"
    rm -rf cmake
}

build_rocksdb() {
    echo "###########################"
    echo "Building RocksDB"
    echo "###########################"
    cd "$ROOT_DIR"
    RocksDB_ROOT="$THIRD_PARTY_DIR/rocksdb"
    RocksDB_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/rocksdb"
    rm -rf "$RocksDB_INSTALL_DIR"
    cd "$RocksDB_ROOT"
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DCMAKE_INSTALL_PREFIX="$RocksDB_INSTALL_DIR"
    pushd cmake/build
    make -j 4
    make install
    popd
}

clean_rocksdb() {
    echo "Cleaning RocksDB"
    rm -rf cmake
}

build_aws_sdk_cpp() {
    echo "###########################"
    echo "Building aws-sdk-cpp"
    echo "###########################"
    cd "$ROOT_DIR"
    aws_sdk_cpp_ROOT="$THIRD_PARTY_DIR/aws-sdk-cpp"
    aws_sdk_cpp_INSTALL_DIR="$THIRD_PARTY_BUILD_INSTALL_DIR/aws-sdk-cpp"
    rm -rf "$aws_sdk_cpp_INSTALL_DIR"
    cd "$aws_sdk_cpp_ROOT"
    mkdir -p cmake/build
    cmake -S . -B cmake/build -DCMAKE_INSTALL_PREFIX="$aws_sdk_cpp_INSTALL_DIR" -DCMAKE_OSX_ARCHITECTURES=$(uname -m)
    pushd cmake/build
    make -j 4
    make install
    popd
    cp "$aws_sdk_cpp_INSTALL_DIR/lib/cmake/AWSSDK/build/aws-sdk-cpp-config.cmake" "$aws_sdk_cpp_INSTALL_DIR/lib/cmake/aws-sdk-cpp-config.cmake"
}

clean_aws_sdk_cpp() {
    echo "Cleaning aws-sdk-cpp"
    rm -rf cmake
}

case "$MY_BUILD_TARGET" in
    all)
        build_cmake
        build_boost
        build_spdlog
        build_grpc
        build_gflags
        build_rocksdb
        build_aws_sdk_cpp
        ;;
    cmake)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_cmake
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_cmake
        fi
        ;;
    boost)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_boost
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_boost
        fi
        ;;
    spdlog)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_spdlog
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_spdlog
        fi
        ;;
    grpc)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_grpc
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_grpc
        fi
        ;;
    gflags)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_gflags
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_gflags
        fi
        ;;
    rocksdb)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_rocksdb
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_rocksdb
        fi
        ;;
    aws-sdk-cpp)
        if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
            build_aws_sdk_cpp
        elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
            clean_aws_sdk_cpp
        fi
        ;;
    minio-cpp)
        echo "###########################"
        echo "Building miniocpp (not implemented)"
        echo "###########################"
        ;;
    *)
        usage
        exit 1
        ;;
esac

echo "-------------------------------------"
echo "Finish CPPIO bootstrap.sh"
echo "-------------------------------------"
