#!/bin/bash
echo "-------------------------------------"
echo "To run cppio makecppio.sh"
echo "-------------------------------------"

VERBOSE=false

while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        *)
            shift
            ;;
    esac
done

if [[ "$CLEAN" = true ]]; then
    rm -rf build/cppio
fi

if [[ "$VERBOSE" = true ]]; then
    cmake -S . -B build/cppio -D CMAKE_BUILD_TYPE=Debug --trace-expand
else
    cmake -S . -B build/cppio -D CMAKE_BUILD_TYPE=Debug
fi
cmake --build build/cppio --parallel $(nproc)
cmake --install build/cppio --prefix build/cppio/cppio_install_root

echo "-------------------------------------"
echo "Finish cppio makecppio.sh"
echo "-------------------------------------"
