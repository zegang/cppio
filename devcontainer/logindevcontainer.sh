#!/bin/bash
# Copyright 2026 cppio authors. All rights reserved.

# --- Configuration ---
CPPIO_PROJECT_NAME="cppio"
IMAGE_NAME="${CPPIO_PROJECT_NAME}/cppio-dev"
DOCKERFILE_PATH=$(dirname -- "$(realpath "$0")")
echo "The absolute current directory (using realpath) is: $DOCKERFILE_PATH"
ENGINE="docker"
# ---------------------

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -e|--engine)
            ENGINE="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $(basename "$0") [-e|--engine ENGINE] [-h|--help]"
            echo "  -e, --engine    Specify container engine: docker or podman (default: docker)."
            echo "  -h, --help      Show this help message."
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

log_into_container() {
    local engine=$1
    echo "Log into container ${IMAGE_NAME} by $engine"
    $engine container start ${IMAGE_NAME}
    echo "Command: $engine exec -it ${IMAGE_NAME} /bin/bash"
    $engine exec -it ${IMAGE_NAME} /bin/bash
}

if [ "$ENGINE" = "docker" ]; then
    if ! command -v docker &> /dev/null; then
        echo "Docker not found. Please install Docker."
        exit 1
    fi
    log_into_container docker
elif [ "$ENGINE" = "podman" ]; then
    if ! command -v podman &> /dev/null; then
        echo "Podman not found. Please install Podman."
        exit 1
    fi
    log_into_container podman
else
    echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
    exit 1
fi
