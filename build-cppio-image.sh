#!/bin/bash

# Bash script to automate building cppio-rel for Docker Compose on Linux
# This script builds the project in the dev container, then runs docker-compose build

# --- Configuration ---
DEV_IMAGE_NAME="cppio-dev"
IMAGE_NAME="cppio-rel"
BUILD_CONTAINER_NAME="cppio_dev_build_temp"
ENGINE="docker"

CPPIO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPPIO_DEPLOY_ROOT="${CPPIO_ROOT}/deploy"
DOCKERFILE_PATH="${CPPIO_DEPLOY_ROOT}/SuseContainerImageFile"
WORKSPACE_PATH="${CPPIO_ROOT}"
# ---------------------

# Show help
show_help() {
    echo "Usage: ./build-cppio-image.sh [OPTIONS]"
    echo "This script automates the build process for CPPIO image ${IMAGE_NAME}:"
    echo "1. Checks if the CPPIO dev image $DEV_IMAGE_NAME exists"
    echo "2. Builds CPPIO project inside a tmp dev container (image $DEV_IMAGE_NAME)"
    echo "3. Runs docker-compose build cppio-hsd"
    echo ""
    echo "OPTIONS:"
    echo "  -e, --engine ENGINE    Specify container engine: docker or podman (default: docker)."
    echo "  -h, --help             Show this help message"
}

# Function to check if image exists
image_exists() {
    local image_name=$1
    if [ "$ENGINE" = "docker" ]; then
        docker image inspect "${image_name}:latest" &> /dev/null
        return $?
    elif [ "$ENGINE" = "podman" ]; then
        podman image inspect "${image_name}:latest" &> /dev/null
        return $?
    else
        echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
        return 1
    fi
}

# Function to check if container is running
container_running() {
    local container_name=$1
    if command -v docker &> /dev/null; then
        local output=$(docker ps --filter "name=${container_name}" --format "{{.Names}}" 2>/dev/null)
        [ "${output}" = "${container_name}" ]
        return $?
    elif command -v podman &> /dev/null; then
        local output=$(podman ps --filter "name=${container_name}" --format "{{.Names}}" 2>/dev/null)
        [ "${output}" = "${container_name}" ]
        return $?
    else
        echo "Neither Docker nor Podman is installed."
        return 1
    fi
}

# Function to build CPPIO dev image
build_cppio_dev_image() {
    echo "Building CPPIO dev image ${DEV_IMAGE_NAME}..."
    local start_dev_script="${CPPIO_ROOT}/devcontainer/startdevcontainer.sh"
    
    if [ ! -f "${start_dev_script}" ]; then
        echo "Error: ${start_dev_script} not found."
        return 1
    fi
    
    bash "${start_dev_script}" -e "${ENGINE}" -b
    if [ $? -eq 0 ]; then
        echo "Dev image built successfully!"
        return 0
    else
        echo "Failed to build dev image."
        return 1
    fi
}

# Function to run build in container
run_build_in_container() {
    echo "Running build in dev container..."
    local cppio_build_commands="cd /workspace/cppio && ./bootstrap.sh all && ./makecppio.sh"
    
    local cmd="${ENGINE} run --rm --name ${BUILD_CONTAINER_NAME} -v ${WORKSPACE_PATH}:/workspace/cppio ${DEV_IMAGE_NAME} /bin/bash -c \"${cppio_build_commands}\""
    echo "Command: ${cmd}"
    eval "${cmd}"
    
    if [ $? -eq 0 ]; then
        echo "Build in container successful!"
        return 0
    else
        echo "Build in container failed."
        return 1
    fi
}

# Function to run docker-compose build
run_compose_build() {
    echo "Running docker-compose build..."
    
    if ! command -v docker-compose &> /dev/null && ! command -v docker &> /dev/null; then
        echo "Error: docker-compose or docker not found."
        return 1
    fi
    
    local compose_cmd="docker-compose"
    if ! command -v docker-compose &> /dev/null; then
        compose_cmd="docker compose"
    fi
    
    local cmd="cd ${CPPIO_DEPLOY_ROOT} && ${compose_cmd} build cppio-hsd"
    echo "Command: ${cmd}"
    eval "${cmd}"
    
    if [ $? -eq 0 ]; then
        echo "Docker-compose build successful!"
        return 0
    else
        echo "Docker-compose build failed."
        return 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -e|--engine)
            ENGINE="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Main script execution
echo "DOCKERFILE_PATH: ${DOCKERFILE_PATH}, WORKSPACE_PATH: ${WORKSPACE_PATH}"
echo "Starting automated build process for cppio-rel..."

# Check if dev image exists, build if not
if ! image_exists "${DEV_IMAGE_NAME}"; then
    echo "Image ${DEV_IMAGE_NAME} does not exist."
    if ! build_cppio_dev_image; then
        echo "Failed to build image ${DEV_IMAGE_NAME}. Exiting."
        exit 1
    fi
fi

# Do CPPIO build in dev container
if ! run_build_in_container; then
    echo "Build in container failed. Exiting."
    exit 1
fi

# Run docker-compose build
if ! run_compose_build; then
    echo "Docker-compose build failed. Exiting."
    exit 1
fi

echo "All steps completed successfully!"
