#!/bin/bash
# install-helm-podman-kube.sh
# Usage: ./install-helm-podman-kube.sh apply <chart-package.tgz> <release_name> <namespace>
# Example: ./install-helm-podman-kube.sh apply ../packages/cppio-app-0.1.0.tgz cppio-app default

if ! command -v podman &> /dev/null; then
    echo "Podman is not installed. Please install Podman."
    exit 1
fi
if ! command -v helm &> /dev/null; then
    echo "Helm is not installed. Please install Helm."
    exit 1
fi

CURRENT_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
COMMAND="$1"
PKG_FILE="${2:-$CURRENT_DIR/packages/cppio-app-0.0.1.tgz}"
RELEASE_NAME="${3:-cppio-app}"
NAMESPACE="${4:-default}"

print_usage() {
    echo "Usage: $0 <command> <chart-package.tgz> <release_name> <namespace>"
    echo "Commands:"
    echo "  apply         Render and apply the Helm package to Podman Kubernetes (default)"
    echo "  down          Delete pods from a manifest file: $0 down <manifest.yaml>"
    echo "  upload-image  Upload a local Podman image to the Kubernetes cluster: $0 upload-image <image_name>"
    echo "  unregister-image Unregister and delete an image from the Kubernetes cluster: $0 unregister-image <image_name>"
    echo "  help          Show this help message"
    echo "Examples:"
    echo "  $0 apply ../packages/cppio-app-0.1.0.tgz cppio-app default"
    echo "  $0 down manifest.yaml"
}

if [ "$1" = "help" ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    print_usage
    exit 0
fi

if [ -z "$1" ]; then
    print_usage
    exit 1
fi

apply_pkg() {
    PKG_FILE="$1"
    RELEASE_NAME="$2"
    NAMESPACE="$3"
    if [ -z "$PKG_FILE" ] || [ ! -f "$PKG_FILE" ]; then
        echo "Helm package file not found: $PKG_FILE"
        exit 1
    fi
    TMP_DIR=$(mktemp -d -t cppio-app.XXXXXX)
    helm template "$RELEASE_NAME" "$PKG_FILE" -n "$NAMESPACE" > "$TMP_DIR/manifest.yaml"
    echo "Copying manifest $TMP_DIR/manifest.yaml to kind-control-plane and applying with kubectl..."
    podman cp "$TMP_DIR/manifest.yaml" kind-control-plane:/tmp/cppio-app-manifest.yaml
    podman exec kind-control-plane kubectl apply -f /tmp/cppio-app-manifest.yaml --namespace "$NAMESPACE"
    echo "Done."
}

down_pkg() {
    YAML_FILE="$1"
    NAMESPACE="$3"
    if [ -z "$YAML_FILE" ] || [ ! -f "$YAML_FILE" ]; then
        podman exec kind-control-plane kubectl delete -f /tmp/cppio-app-manifest.yaml --namespace "$NAMESPACE"
    else
        echo "Copying manifest to kind-control-plane and deleting with kubectl..."
        podman cp "$YAML_FILE" kind-control-plane:/tmp/cppio-app-manifest.yaml
        podman exec kind-control-plane kubectl delete -f /tmp/cppio-app-manifest.yaml --namespace "$NAMESPACE"
    fi
    echo "Resource deletion complete."
}

# Function to upload and register a local Podman image into kind-control-plane's k8s
upload_image_to_kind() {
    IMAGE_NAME="${1:-cppio_rel:latest}"
    KIND_NODE="kind-control-plane"
    echo "Saving local image $IMAGE_NAME to tar..."
    podman save -o /tmp/cppio_rel_image.tar "$IMAGE_NAME"
    echo "Copying image tar to $KIND_NODE:/tmp/cppio_rel_image.tar ..."
    podman cp /tmp/cppio_rel_image.tar $KIND_NODE:/tmp/cppio_rel_image.tar
    echo "Loading image into kind node..."
    podman exec $KIND_NODE ctr --namespace k8s.io images import /tmp/cppio_rel_image.tar
    echo "Image $IMAGE_NAME registered in kind-control-plane's k8s."
    # Clean up
    echo "Cleaning up temporary files..."
    rm -f /tmp/cppio_rel_image.tar
    podman exec $KIND_NODE rm -f /tmp/cppio_rel_image.tar
}

# Function to unregister and delete an image from kind-control-plane's k8s
unregister_image_from_kind() {
    IMAGE_NAME="${1:-cppio_rel:latest}"
    KIND_NODE="kind-control-plane"
    # Get image ID in containerd (k8s.io namespace)
    podman exec $KIND_NODE ctr --namespace k8s.io images list | grep "$IMAGE_NAME"
    IMAGE_REL_NAME=$(podman exec $KIND_NODE ctr --namespace k8s.io images list | grep "$IMAGE_NAME" | awk '{print $1}')
    if [ -z "$IMAGE_REL_NAME" ]; then
        echo "Image $IMAGE_NAME not found in kind-control-plane's k8s."
        return 1
    fi
    echo "Unregistering and deleting image $IMAGE_REL_NAME from kind-control-plane's k8s..."
    podman exec $KIND_NODE ctr --namespace k8s.io images remove "$IMAGE_REL_NAME"
    echo "Image $IMAGE_REL_NAME removed from kind-control-plane's k8s."
}

if [ "$1" = "apply" ]; then
    apply_pkg "$PKG_FILE" "$RELEASE_NAME" "$NAMESPACE"
    exit 0
fi

if [ "$1" = "down" ]; then
    down_pkg "$2"
    exit 0
fi

if [ "$1" = "upload-image" ]; then
    upload_image_to_kind "${2:-cppio_rel:latest}"
    exit 0
fi

if [ "$1" = "unregister-image" ]; then
    unregister_image_from_kind "${2:-cppio_rel:latest}"
    exit 0
fi

print_usage
exit 1
