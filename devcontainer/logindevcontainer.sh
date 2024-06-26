#!/bin/bash

# --- Configuration ---
IMAGE_NAME="cppio_dev"
DOCKERFILE_PATH=$(dirname -- "$(realpath "$0")")
echo "The absolute current directory (using realpath) is: $DOCKERFILE_PATH"
# ---------------------

log_into_container() {
    local engine=$1
    echo "Log into container ${IMAGE_NAME} by $engine"
    $engine container start ${IMAGE_NAME}
    echo "Command: $engine exec -it ${IMAGE_NAME} /bin/bash"
    $engine exec -it ${IMAGE_NAME} /bin/bash
}

if command -v docker &> /dev/null; then
    log_into_container docker
elif command -v podman &> /dev/null; then
    log_into_container podman
else
    echo "Neither Docker nor Podman found."
    exit 1
fi
