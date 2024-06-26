#!/bin/bash

############## Build Container Image ############# 

IMAGE_NAME="cppio_rel"
DOCKERFILE="SuseContainerImageFile"
SCRIPT_PATH="$(realpath "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"
CONTEXT_DIR="$(git -C "$SCRIPT_DIR" rev-parse --show-toplevel 2>/dev/null || echo "$PARENT_DIR")"
echo "Using context directory: $CONTEXT_DIR"

build_image() {
    if command -v podman &> /dev/null; then
        echo "Using podman to build the image..."
        podman build --network=host -t "$IMAGE_NAME" -f "$DOCKERFILE" "$CONTEXT_DIR"
    elif command -v docker &> /dev/null; then
        echo "Using docker to build the image..."
        docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$CONTEXT_DIR"
    elif command -v nerdctl &> /dev/null; then
        echo "Using nerdctl to build the image..."
        nerdctl build -t "$IMAGE_NAME" -f "$DOCKERFILE" "$CONTEXT_DIR"
    else
        echo "No supported container tool (podman, docker, nerdctl) found. Please install one."
        exit 1
    fi
}

build_image