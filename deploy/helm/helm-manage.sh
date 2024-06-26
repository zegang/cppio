#!/bin/bash

# Usage: ./helm-manage.sh [install|upgrade|uninstall|template|build] [release_name] [namespace]
# Default release_name: $CHART_NAME
# Default namespace: default

set -e

if ! command -v helm &> /dev/null; then
    echo "Helm is not installed."
    read -p "Do you want to install Helm now? (y/n) " install_helm
    if [ "$install_helm" = "y" ]; then
        echo "Installing Helm..."
        if command -v curl &> /dev/null; then
            curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
            chmod 700 get_helm.sh
            ./get_helm.sh
            rm -f get_helm.sh
        elif command -v wget &> /dev/null; then
            wget -O get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
            chmod 700 get_helm.sh
            ./get_helm.sh
            rm -f get_helm.sh
        else
            echo "Neither curl nor wget is available to download Helm. Please install one and try again."
            exit 1
        fi
    else
        echo "Helm is required to manage the chart. Exiting."
        exit 1
    fi
fi

CURRENT_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
echo "----------------------------------"
echo "Current directory: $CURRENT_DIR"
echo "----------------------------------"

CHART_NAME="cppio-app"
CHART_DIR="$CURRENT_DIR/$CHART_NAME"
RELEASE_NAME="${2:-$CHART_NAME}"
NAMESPACE="${3:-default}"
PKG_DIR="$CHART_DIR/../packages"

if [ ! -d "$CHART_DIR" ]; then
    echo "Helm chart directory '$CHART_NAME' not found in the current directory."
    exit 1
fi

find_helm_package() {
    pkg_file=$(ls -t "$PKG_DIR"/$CHART_NAME-*.tgz 2>/dev/null | head -n1)
    if [ -z "$pkg_file" ]; then
      echo "No packaged chart found in $pkg_file"
      exit 1
    fi
    echo "$pkg_file"
}

case "$1" in
    install)
        echo "Installing Helm chart..."
        helm install "$RELEASE_NAME" "$CHART_DIR" -n "$NAMESPACE" --create-namespace
        ;;
    upgrade)
        echo "Upgrading Helm chart..."
        helm upgrade "$RELEASE_NAME" "$CHART_DIR" -n "$NAMESPACE"
        ;;
    uninstall)
        echo "Uninstalling Helm release..."
        helm uninstall "$RELEASE_NAME" -n "$NAMESPACE"
        ;;
    template)
        echo "Rendering Helm templates..."
        helm template "$RELEASE_NAME" "$CHART_DIR" -n "$NAMESPACE"
        ;;
    build-pkg)
        echo "Packaging Helm chart..."
        helm package "$CHART_DIR" -d "$PKG_DIR"
        ;;
    info-pkg)
        PKG_FILE=$(find_helm_package)
        echo "Helm package file: $PKG_FILE"
        helm show all "$PKG_FILE"
        ;;
    list-pkg)
        echo "Listing packaged Helm charts in $PKG_DIR:"
        if [ ! -d "$PKG_DIR" ]; then
            exit 0
        fi
        ls -loh "$PKG_DIR"/$CHART_NAME-*.tgz 2>/dev/null
        ;;
    clean-pkg)
        echo "Cleaning up packaged Helm charts..."
        rm -f "$PKG_DIR"/$CHART_NAME-*.tgz 2>/dev/null
        ;;
    install-pkg)
        echo "Installing Helm chart from package..."
        PKG_FILE=$(find_helm_package)
        helm install "$RELEASE_NAME" "$PKG_FILE" -n "$NAMESPACE" --create-namespace
        ;;
    upgrade-pkg)
        echo "Upgrading Helm chart from package..."
        PKG_FILE=$(find_helm_package)
        helm upgrade "$RELEASE_NAME" "$PKG_FILE" -n "$NAMESPACE"
        ;;
    uninstall-pkg)
        helm uninstall "$RELEASE_NAME" -n "$NAMESPACE"
        ;;
    *)
        echo "Usage: $0 <command> [release_name] [namespace]"
        echo "Commands:"
        echo "  install        Install the chart"
        echo "  upgrade        Upgrade the chart"
        echo "  uninstall      Uninstall the release"
        echo "  template       Render templates"
        echo "  build-pkg      Package the chart"
        echo "  info-pkg       Show information about the packaged chart"
        echo "  list-pkg       List packaged charts"
        echo "  clean-pkg      Clean up packaged charts"
        echo "  install-pkg    Install from packaged chart"
        echo "  upgrade-pkg    Upgrade from packaged chart"
        echo "  uninstall-pkg  Uninstall the release (pkg)"
        exit 1
        ;;
esac