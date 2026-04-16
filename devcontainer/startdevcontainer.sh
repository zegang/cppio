#!/bin/bash

# --- Configuration ---
IMAGE_NAME="cppio_dev"
DOCKERFILE_PATH=$(dirname -- "$(realpath "$0")")
echo "The absolute current directory (using realpath) is: $DOCKERFILE_PATH"
# ---------------------

ENGINE="docker"
DO_BUILD=false
DO_START=false
RM_CONTAINER=false
LIST_CONTAINER=false

list_containers() {
    if command -v docker &> /dev/null; then
        echo "Listing Docker containers (all):"
        docker ps -a
    elif command -v podman &> /dev/null; then
        echo "Listing Podman containers (all):"
        podman ps -a
    else
        echo "Neither Docker nor Podman is installed."
        return 1
    fi
}

# --- Proxy Check and Set ---
proxy_check_set() {
    local no_proxy_list="localhost,127.0.0.1,::1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16,fc00::/7,*.local"
    if [ -z "$HTTP_PROXY" ] && [ -z "$http_proxy" ] || [ -z "$HTTPS_PROXY" ] && [ -z "$https_proxy" ]; then
        echo "No HTTP or HTTPS proxy is set in the environment."
        read -p "Do you want to set HTTP/HTTPS proxy for this session? (y/n) " set_proxy
        if [ "$set_proxy" = "y" ]; then
            read -p "Enter HTTP proxy URL (e.g., http://proxy.example.com:8080, leave blank to skip): " proxy_url
            if [ ! -z "$proxy_url" ]; then
                if [[ "$proxy_url" != http://* ]]; then
                    proxy_url="http://$proxy_url"
                fi
                export HTTP_PROXY="$proxy_url"
                export http_proxy="$proxy_url"
                export NO_PROXY="$no_proxy_list"
                export no_proxy="$no_proxy_list"
                echo "HTTP proxy set for this session: $proxy_url"
                echo "NO_PROXY set for local and private network addresses."
            fi
            read -p "Enter HTTPS proxy URL (e.g., https://proxy.example.com:8443, leave blank to skip): " proxy_url_s
            if [ ! -z "$proxy_url_s" ]; then
                if [[ "$proxy_url_s" != http://* ]]; then
                    proxy_url_s="http://$proxy_url_s"
                fi
                export HTTPS_PROXY="$proxy_url_s"
                export https_proxy="$proxy_url_s"
                export NO_PROXY="$no_proxy_list"
                export no_proxy="$no_proxy_list"
                echo "HTTPS proxy set for this session: $proxy_url_s"
                echo "NO_PROXY set for local and private network addresses."
            fi
        fi
    else
        current_http_proxy="${HTTP_PROXY:-$http_proxy}"
        current_https_proxy="${HTTPS_PROXY:-$https_proxy}"
        echo "Current HTTP proxy: $current_http_proxy"
        echo "Current HTTPS proxy: $current_https_proxy"
        read -p "Do you want to unset HTTP/HTTPS proxy for this session? (y/n) " unset_proxy
        if [ "$unset_proxy" = "y" ]; then
            unset HTTP_PROXY http_proxy HTTPS_PROXY https_proxy NO_PROXY no_proxy
            echo "HTTP/HTTPS proxy variables have been unset for this session."
        fi
    fi
}

# Function to build with Docker
build_with_docker() {
    echo "Attempting to build with Docker..."
    echo "Command: docker build -t $IMAGE_NAME -f $DOCKERFILE_PATH/Dockerfile-$(uname -m) $DOCKERFILE_PATH"
    docker build -t "$IMAGE_NAME" -f $DOCKERFILE_PATH/Dockerfile-$(uname -m) $DOCKERFILE_PATH
    if [ $? -eq 0 ]; then
        echo "Docker build successful!"
        return 0 # Success
    else
        echo "Docker build failed."
        return 1 # Failure
    fi
}

# Function to build with Podman
build_with_podman() {
    echo "Attempting to build with Podman..."
    echo "Command: podman build --network=host -t $IMAGE_NAME -f $DOCKERFILE_PATH/Dockerfile-$(uname -m) $DOCKERFILE_PATH"
    podman build --network=host -t "$IMAGE_NAME" -f $DOCKERFILE_PATH/Dockerfile-$(uname -m) $DOCKERFILE_PATH
    if [ $? -eq 0 ]; then
        echo "Podman build successful!"
        return 0 # Success
    else
        echo "Podman build failed."
        return 1 # Failure
    fi
}

build_image() {
    if [ "$ENGINE" = "docker" ]; then
        if ! command -v docker &> /dev/null; then
            echo "Docker not found. Please install Docker."
            exit 1
        fi
        build_with_docker
    elif [ "$ENGINE" = "podman" ]; then
        if ! command -v podman &> /dev/null; then
            echo "Podman not found. Please install Podman."
            exit 1
        fi
        build_with_podman
    else
        echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
        exit 1
    fi
}

start_container() {
    local ct_run_parm="--network=host -v ${DOCKERFILE_PATH}/..:/workspace/cppio -it --name ${IMAGE_NAME} ${IMAGE_NAME}:latest"
    if [ "$ENGINE" = "docker" ]; then
        echo "Start container ${IMAGE_NAME} by Docker"
        echo "Command: docker run --privileged ${ct_run_parm}"
        docker run --privileged ${ct_run_parm}
    elif [ "$ENGINE" = "podman" ]; then
        echo "Start container ${IMAGE_NAME} by Podman"
        echo "Command: podman run ${ct_run_parm}"
        podman run ${ct_run_parm}
    else
        echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
        exit 1
    fi
}

remove_container() {
    if [ "$ENGINE" = "docker" ]; then
        echo "Remove container ${IMAGE_NAME} by Docker"
        echo "Command: docker rm ${IMAGE_NAME} --force"
        docker rm ${IMAGE_NAME} --force
    elif [ "$ENGINE" = "podman" ]; then
        echo "Remove container ${IMAGE_NAME} by Podman"
        echo "Command: podman rm ${IMAGE_NAME} --force"
        podman rm ${IMAGE_NAME} --force
    else
        echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
        exit 1
    fi
}

# Function to check if an image exists
image_exists() {
    local image_name="$1"
    if [ "$ENGINE" = "docker" ]; then
        docker image inspect "$image_name:latest" > /dev/null 2>&1
        return $?
    elif [ "$ENGINE" = "podman" ]; then
        podman image inspect "$image_name:latest" > /dev/null 2>&1
        return $?
    else
        echo "Invalid engine: $ENGINE. Please specify 'docker' or 'podman'."
        return 1
    fi
}

while (( "$#" )); do
  case "$1" in
    -e|--engine)
      ENGINE="$2"
      shift 2
      ;;
    -b|--build)
      DO_BUILD=true
      shift
      ;;
    -r|--remove)
      RM_CONTAINER=true
      shift
      ;;
    -s|--start)
      DO_START=true
      shift
      ;;
    -p|--proxy)
      proxy_check_set
      shift
      ;;
    -l|--list)
      LIST_CONTAINER=true
      shift
      ;;
    -h|--help)
      echo "Usage: $(basename "$0") [-e|--engine ENGINE] [-b|--build] [-s|--start] [-r|--remove] [-l|--list] [-p|--proxy] [-h|--help]"
      echo "  -e, --engine    Specify container engine: docker or podman (default: docker)."
      echo "  -b, --build     Build the container image."
      echo "  -s, --start     Start the container."
      echo "  -r, --remove    Remove the container."
      echo "  -l, --list      List all containers."
      echo "  -p, --proxy     Set or unset HTTP/HTTPS proxy."
      echo "  -h, --help      Show this help message."
      exit 0
      ;;
    --) # End of options
      shift
      break
      ;;
    -*) # Handle unsupported flags
      echo "Error: Unsupported flag $1" >&2
      exit 1
      ;;
    *) # Preserve positional arguments
      PARAMS="$PARAMS $1"
      shift
      ;;
  esac
done

# Set the remaining positional arguments
eval set -- "$PARAMS"
echo "Positional arguments: $@"

# Use the option variables and positional arguments
if [ "$DO_BUILD" = true ]; then
  echo "Build Image..."
  build_image
fi

if [ "$RM_CONTAINER" = true ]; then
    echo "Remove Container..."
    remove_container
fi

if [ "$LIST_CONTAINER" = true ]; then
    list_containers
fi

if [ "$DO_START" = true ]; then
    if ! image_exists "$IMAGE_NAME"; then
        echo "Image $IMAGE_NAME does not exist."
        read -p "Do you want to build it? (y/n) " build_choice 
        if [ "$build_choice" = "y" ]; then
            build_image
            if [ $? -ne 0 ]; then
                echo "Failed to build the image. Exiting."
                exit 1
            fi
        else
            echo "Exiting without starting the container."
            exit 0
        fi
    fi
    echo "Start Container..."
    start_container
fi