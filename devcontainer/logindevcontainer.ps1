# PowerShell script to log into the cppio_dev container (Docker or Podman)

param(
    [Alias('e')][string]$Engine = 'docker',
    [Alias('h')][switch]$Help
)

if ($Help) {
    Write-Host "Usage: .\\logindevcontainer.ps1 [-Engine {docker|podman}] [-Help]"
    Write-Host "  -Engine (-e): Specify container engine: docker or podman (default: docker)."
    Write-Host "  -Help (-h): Show this help message."
    exit 0
}

# --- Configuration ---
$IMAGE_NAME = "cppio_dev"
$DOCKERFILE_PATH = Split-Path -Parent (Resolve-Path $MyInvocation.MyCommand.Path)
Write-Host "The absolute current directory (using Resolve-Path) is: $DOCKERFILE_PATH"
# ---------------------

if ($Engine -eq 'docker') {
    if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
        Write-Host "Docker not found. Please install Docker."
        exit 1
    }
    Write-Host "Log into container $IMAGE_NAME by Docker"
    docker container start $IMAGE_NAME | Out-Null
    Write-Host "Command: docker exec -it $IMAGE_NAME /bin/bash"
    docker exec -it $IMAGE_NAME /bin/bash
} elseif ($Engine -eq 'podman') {
    if (-not (Get-Command podman -ErrorAction SilentlyContinue)) {
        Write-Host "Podman not found. Please install Podman."
        exit 1
    }
    Write-Host "Log into container $IMAGE_NAME by Podman"
    podman container start $IMAGE_NAME | Out-Null
    Write-Host "Command: podman exec -it $IMAGE_NAME /bin/bash"
    podman exec -it $IMAGE_NAME /bin/bash
} else {
    Write-Host "Invalid engine: $Engine. Please specify 'docker' or 'podman'."
    exit 1
}
