#!/bin/bash
# deploy-yaml.sh
# Usage: ./deploy-yaml.sh [up|down] <cppio-deployment.yaml>
# Example: ./deploy-yaml.sh up cppio-deployment.yaml
#          ./deploy-yaml.sh down cppio-deployment.yaml

set -e

COMMAND="${1:-deploy}"
YAML_FILE="${2:-cppio-deployment.yaml}"

if command -v podman &> /dev/null; then
    CONTAINER_TOOL="podman"
else
    if command -v docker &> /dev/null; then
        CONTAINER_TOOL="docker"
    else
        echo "Neither Podman nor Docker is installed. Please install one."
        exit 1
    fi
fi

if [ ! -f "$YAML_FILE" ]; then
    echo "YAML file not found: $YAML_FILE"
    exit 1
fi

if [ "$COMMAND" = "up" ]; then
    echo "Deploying $YAML_FILE using $CONTAINER_TOOL kube play..."
    $CONTAINER_TOOL kube play "$YAML_FILE"
    echo "Deployment complete."
elif [ "$COMMAND" = "down" ]; then
    echo "Removing deployment defined in $YAML_FILE using $CONTAINER_TOOL kube down..."
    $CONTAINER_TOOL kube down "$YAML_FILE"
    echo "Removal complete."
else
    echo "Usage: $0 [up|down] <cppio-deployment.yaml>"
    exit 1
fi
