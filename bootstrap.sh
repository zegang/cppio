#!/bin/bash
echo "-------------------------------------"
echo "CPPIO bootstrap.sh"
echo "-------------------------------------"

MY_BUILD_TARGET="all"
SUBMODULE_BUILD_DIR="cppio_build"
RECLONE_SUBMODULES=false
RESET_HARD2UPSTREAM=false
UPDATE_SUBMODULE=false
NUM_CPUS=1

usage() {
    echo "Usage: $0 [target] [sub-target] [--reclone-submodules] [--reset-hard] [--update-submodules]"
    echo "Available targets:"
    echo "  all       - Build all components"
    echo "  cmake     - Build CMake"
    echo "  boost     - Build Boost libraries"
    echo "  spdlog    - Build spdlog"
    echo "  grpc      - Build gRPC"
    echo "  gflags    - Build gflags"
    echo "  rocksdb   - Build RocksDB"
    echo "  minio-cpp - Build MinIO C++ SDK"
    echo "  nuraft    - Build NuRaft"
    echo "  aws-sdk-cpp - Build AWS SDK for C++"
    echo "Sub-targets:"
    echo "  all       - Build all sub-components"
    echo "  install   - Install build outs"
    echo "  clean     - Clean the build"
    echo "  --reclone-submodules  - Remove and re-clone all submodules listed in .gitmodules"
    echo "  --reset-hard - Reset hard and update submodule with its upstream"
    echo "  --update-submodule - Update submodule to its upstream"
}

get_cpu_num() {
    # Check if nproc is available (Linux)
    if command -v nproc >/dev/null 2>&1; then
        NUM_CPUS=$(nproc)
    # Check if sysctl is available (macOS, FreeBSD)
    elif command -v sysctl >/dev/null 2>&1; then
        NUM_CPUS=$(sysctl -n hw.ncpu)
    # Fallback for older Linux or other systems
    elif [ -f /proc/cpuinfo ]; then
        NUM_CPUS=$(grep -c ^processor /proc/cpuinfo)
    fi
}

# Function to remove and re-clone all submodules listed in .gitmodules (parse url and path)
reclone_submodules() {
    echo "###########################"
    echo "Re-cloning all submodules from .gitmodules"
    echo "###########################"
    if [ ! -f .gitmodules ]; then
        echo ".gitmodules file not found!"
        return 1
    fi
    # Parse .gitmodules for path and url pairs
    local paths=()
    local urls=()
    while IFS= read -r line; do
        if [[ $line =~ path\ =\ (.*) ]]; then
            paths+=("${BASH_REMATCH[1]}")
        elif [[ $line =~ url\ =\ (.*) ]]; then
            urls+=("${BASH_REMATCH[1]}")
        fi
    done < .gitmodules
    # Remove and clone each submodule
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

reset_hard_all_to_upstream() {
    git submodule foreach --recursive git clean -fdx
    # Hard reset all submodules to the last committed state
    git submodule foreach --recursive git reset --hard
    # Synchronize submodule URLs in the local configuration
    git submodule sync --recursive
    # Update all submodules to the commit specified by the main repository
    git submodule update --init --recursive --remote --force
}

reset_hard_submodule_to_upstream() {
    echo "Working in directory $(pwd)"
    echo "Target submodule: third_party/$1"
    # Synchronize the submodule's URL from the main repo's configuration
    git submodule sync --recursive -- third_party/$1
    # Update the specified submodule to the commit specified by the main repository
    git submodule update --recursive --init --force --remote -- third_party/$1
}

parse_options() {
    for arg in "$@"; do
        if [ "$arg" == "--reclone-submodules" ]; then
            RECLONE_SUBMODULES=true
        fi
        if [ $"$arg" == "--reset-hard" ]; then
            RESET_HARD2UPSTREAM=true
        fi
        if [ $"$arg" == "--update-submodule" ]; then
            UPDATE_SUBMODULE=true
        fi
    done

    if [ "$RECLONE_SUBMODULES" = true ]; then
        reclone_submodules
        exit 0
    fi

    if [ -z "$1" ]; then
        usage
        exit 1
    else
        MY_BUILD_TARGET="$1"
    fi

    if [ "$RESET_HARD2UPSTREAM" = true ]; then
        if [ "$MY_BUILD_TARGET" == "all" ]; then
            reset_hard_all_to_upstream
        else
            reset_hard_submodule_to_upstream $MY_BUILD_TARGET
        fi
        exit 0
    fi

    if [ -z "$2" ]; then
        MY_BUILD_SUB_TARGET="all"
    else
        MY_BUILD_SUB_TARGET="$2"
    fi
}

# Parse arguments for --reclone-submodules
parse_options "$@"

ROOT_DIR="$(git rev-parse --show-toplevel 2>/dev/null || echo "$(pwd)")"
CXXSTD="c++20"
CXXFLAGS="-O2 -g -std=${CXXSTD}"

THIRD_PARTY_DIR="${ROOT_DIR}/third_party"
THIRD_PARTY_BUILD_INSTALL_DIR="${ROOT_DIR}/build/third_party"

echo -e "ROOT_DIR ${ROOT_DIR}"
echo -e "THIRD_PARTY_DIR ${THIRD_PARTY_DIR}"

if [ "${UPDATE_SUBMODULE}" == "true" ]; then
    echo "###########################"
    echo "Updating Submodules ${MY_BUILD_TARGET}"
    echo "###########################"
    if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
        git submodule sync --recursive
        if [ "$MY_BUILD_TARGET" == "all" ]; then
            git submodule update --init --recursive
        else
            git submodule update --init --recursive third_party/${MY_BUILD_TARGET}
        fi
    fi
fi

############# cmake ##############
# if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "cmake" ]; then
#     echo "###########################"
#     echo "Building CMake"
#     echo "###########################"
#     cd ${ROOT_DIR}
#     CMAKE_SRC="${THIRD_PARTY_DIR}/cmake"
#     CMAKE_INSTALL_DIR="${THIRD_PARTY_BUILD_INSTALL_DIR}/cmake"
#     rm -rf ${CMAKE_INSTALL_DIR}
#     cd ${CMAKE_SRC}
#     if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
#         git checkout v3.31.8
#         ./bootstrap
#         make
#         make install
#     elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
#         echo "Cleaning CMake"
#         mmake uninstall
#     fi
# fi

############# Boost ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "boost" ]; then
    echo "###########################"
    echo "Building Boost"
    echo "###########################"
    cd ${ROOT_DIR}
    BOOST_SRC="${THIRD_PARTY_DIR}/boost"
    BOOST_INSTALL_DIR="${THIRD_PARTY_BUILD_INSTALL_DIR}/boost"
    rm -rf ${BOOST_INSTALL_DIR}
    cd ${BOOST_SRC}
    echo "Working in directory $(pwd)"
    if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
        # Bootstrap Boost.Build
        ./bootstrap.sh --prefix=${BOOST_INSTALL_DIR}
        # Build Boost libraries (adjust --with options as needed)
        ./b2 --prefix=${BOOST_INSTALL_DIR} --with-program_options --with-beast --with-asio --with-any toolset=gcc-14 cxxflags=${CXXFLAGS}
        # ./b2 --prefix=${INSTALL_DIR} --with-program_options link=static runtime-link=static
        # Install Boost libraries
        ./b2 install
    elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
        echo "Cleaning Boost"
    fi
fi


do_cmake_submodule() {
    subm_name="$1"
    subm_cmake_options="$2"
    subm_root="${THIRD_PARTY_DIR}/${subm_name}"
    subm_install_dir="${THIRD_PARTY_BUILD_INSTALL_DIR}/${subm_name}"
    echo "###########################"
    echo "Building ${subm_name}"
    echo "###########################"
    cd ${subm_root}
    echo "Working in directory $(pwd)"
    if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
        rm -rf ${SUBMODULE_BUILD_DIR}
        mkdir ${SUBMODULE_BUILD_DIR}
        cmake -S . -B ${SUBMODULE_BUILD_DIR} -DCMAKE_INSTALL_PREFIX="${subm_install_dir}" \
            ${subm_cmake_options}
        pushd ${SUBMODULE_BUILD_DIR} > /dev/null
        echo "Working in directory $(pwd)"
        make -j ${NUM_CPUS}
        echo "Before install new builds, delete old installs under ${subm_install_dir}"
        rm -rf "${subm_install_dir}"
        echo "Install new builds to ${subm_install_dir}..."
        make -j${NUM_CPUS} install
        popd
    elif [ "$MY_BUILD_SUB_TARGET" == "install" ]; then
        pushd ${SUBMODULE_BUILD_DIR} > /dev/null
        echo "Working in directory $(pwd)"
        echo "Before install new builds, delete old installs under ${subm_install_dir}"
        rm -rf "${subm_install_dir}"
        echo "Install new builds to ${subm_install_dir}..."
        make -j${NUM_CPUS} install
        popd
    elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
        echo "Cleaning ${subm_name}"
        rm -rf ${SUBMODULE_BUILD_DIR}
    fi
}

############# spdlog ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "spdlog" ]; then
    do_cmake_submodule "spdlog"
fi

############# gRPC ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "grpc" ]; then
    do_cmake_submodule grpc "-DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_CXX_STANDARD=17"
    echo "###########################"
    echo "Testing GRPC helloworld"
    echo "###########################"
    cd ${GRPC_ROOT}/examples/cpp/helloworld
    if [ "$MY_BUILD_SUB_TARGET" == "all" ]; then
        rm -rf ${SUBMODULE_BUILD_DIR}
        mkdir -p ${SUBMODULE_BUILD_DIR}
        echo -e "-DProtobuf_DIR ${GRPC_INSTALL_DIR}/lib/cmake/protobuf\n"
        cmake -S . -B ${SUBMODULE_BUILD_DIR} \
            -DCMAKE_PREFIX_PATH="${GRPC_INSTALL_DIR}/lib/cmake/" \
            -DCMAKE_INSTALL_PREFIX="${GRPC_INSTALL_DIR}/examples/cpp/helloworld"
        pushd ${SUBMODULE_BUILD_DIR}
        make -j 4
        echo "*************************************"
        set -x
        ./greeter_server 2>&1 &
        grpcServerPID=$!
        echo "grpcServerPID=${grpcServerPID}"
        sleep 5s
        ./greeter_client
        sleep 5s
        kill ${grpcServerPID}
        set +x
        echo "*************************************"
        popd
    elif [ "$MY_BUILD_SUB_TARGET" == "clean" ]; then
        echo "Cleaning GRPC helloworld"
        rm -rf cmake
    fi
fi

############# gflags ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "gflags" ]; then
    do_cmake_submodule "gflags" "-DBUILD_SHARED_LIBS=ON"
fi
############# RocksDB ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "rocksdb" ]; then
    do_cmake_submodule "rocksdb"
fi
############# Minio-CPP ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "minio-cpp" ]; then
    do_cmake_submodule "minio-cpp"
fi

############# NuRaft ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "nuraft" ]; then
    do_cmake_submodule "nuraft"
fi

############# aws-sdk-cpp ##############
if [ "$MY_BUILD_TARGET" == "all" ] || [ "$MY_BUILD_TARGET" == "aws-sdk-cpp" ]; then
    do_cmake_submodule "aws-sdk-cpp" "-DCMAKE_OSX_ARCHITECTURES=$(uname -m) -DBUILD_ONLY='s3'"
    if [ "$MY_BUILD_SUB_TARGET" == "all" ] || [ "$MY_BUILD_SUB_TARGET" == "install" ]; then
        aws_sdk_cpp_INSTALL_DIR="${THIRD_PARTY_BUILD_INSTALL_DIR}/aws-sdk-cpp"
        cp ${THIRD_PARTY_DIR}/aws-sdk-cpp/${SUBMODULE_BUILD_DIR}/aws-sdk-cpp-config.cmake \
           ${aws_sdk_cpp_INSTALL_DIR}/lib/cmake/aws-sdk-cpp-config.cmake
    fi
fi

############# Back to Root DIR ##############
cd ${ROOT_DIR}

echo "-------------------------------------"
echo "Finish CPPIO bootstrap.sh"
echo "-------------------------------------"
