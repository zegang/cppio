# PowerShell script to automate building cppio-rel for Docker Compose on Windows 11
# This script builds the project in the dev container, then runs docker-compose build in WSL

param(
    [Alias('e')][string]$Engine = 'docker',
    [Alias('h')][switch]$Help
)

# --- Configuration ---
$DEV_IMAGE_NAME = "cppio-dev"
$IMAGE_NAME = "cppio-rel"
$BUILD_CONTAINER_NAME = "cppio_dev_build_temp"
$WSL_DISTRIBUTION_NAME = "Ubuntu-24.04"

$CPPIO_ROOT = Split-Path -Parent (Resolve-Path $MyInvocation.MyCommand.Path)
$CPPIO_DEPLOY_ROOT = Join-Path $CPPIO_ROOT "deploy"
$DOCKERFILE_PATH = Join-Path $CPPIO_DEPLOY_ROOT "SuseContainerImageFile"
$WORKSPACE_PATH = $CPPIO_ROOT
# ---------------------

if ($Help) {
    Write-Host "Usage: .\\build-cppio-image.ps1 [-Engine {docker|podman}] [-Help]"
    Write-Host "This script automates the build process for CPPIO image ${IMAGE_NAME}:"
    Write-Host "1. Checks if the CPPIO dev image $DEV_IMAGE_NAME exists"
    Write-Host "2. Builds CPPIO project inside a tmp dev container (image $DEV_IMAGE_NAME)"
    Write-Host "3. Runs docker-compose build cppio-hsd"
    Write-Host ""
    Write-Host "OPTIONS:"
    Write-Host "  -Engine (-e)    Specify container engine: docker or podman (default: docker)."
    Write-Host "  -Help (-h)      Show this help message."
    exit 0
}

# Function to check if image exists
function Image-Exists {
    param([string]$imageName)
    if ($Engine -eq 'docker') {
        $null = docker image inspect "${imageName}:latest" 2>$null
        return $LASTEXITCODE -eq 0
    } elseif ($Engine -eq 'podman') {
        $null = podman image inspect "${imageName}:latest" 2>$null
        return $LASTEXITCODE -eq 0
    } else {
        Write-Host "Invalid engine: $Engine. Please specify 'docker' or 'podman'."
        return $false
    }
}

# Function to check if container is running
function Container-Running {
    param([string]$containerName)
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        $output = docker ps --filter "name=$containerName" --format "{{.Names}}"
        return $output -eq $containerName
    } elseif (Get-Command podman -ErrorAction SilentlyContinue) {
        $output = podman ps --filter "name=$containerName" --format "{{.Names}}"
        return $output -eq $containerName
    } else {
        Write-Host "Neither Docker nor Podman is installed."
        return $false
    }
}

# Function to build image (copied from startdevcontainer.ps1)
function Build-CPPIO-Dev-Image {
    Write-Host "Building CPPIO dev image $DEV_IMAGE_NAME..."
    $startDevScript = Join-Path $CPPIO_ROOT "devcontainer" "startdevcontainer.ps1"
    & $startDevScript -e $Engine -b
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Dev image built successfully!"
        return $true
    } else {
        Write-Host "Failed to build dev image."
        return $false
    }
}

# Function to run build in container
function Run-Build-In-Container {
    Write-Host "Running build in dev container..."
    $CPPIO_BUILD_COMMANDS = "cd /workspace/cppio && ./bootstrap.sh all && ./makecppio.sh"
    $cmd = "$Engine run --rm --name $BUILD_CONTAINER_NAME -v ${WORKSPACE_PATH}:/workspace/cppio $DEV_IMAGE_NAME /bin/bash -c `"${CPPIO_BUILD_COMMANDS}`""
    Write-Host "Command: $cmd"
    Invoke-Expression $cmd
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build in container successful!"
        return $true
    } else {
        Write-Host "Build in container failed."
        return $false
    }
}

# Function to run docker-compose build in WSL
function Run-Compose-Build-In-WSL {
    Write-Host "Running docker-compose build in WSL..."
    $composeCmd = "cd /mnt/d/cppio/deploy && docker-compose build cppio-hsd"
    $wslCmd = "wsl -d $WSL_DISTRIBUTION_NAME -e bash -c `"${composeCmd}`""
    Write-Host "Command: $wslCmd"
    Invoke-Expression $wslCmd
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Docker-compose build successful!"
        return $true
    } else {
        Write-Host "Docker-compose build failed."
        return $false
    }
}

# Main script
Write-Host "DOCKERFILE_PATH: $DOCKERFILE_PATH, WORKSPACE_PATH: $WORKSPACE_PATH"
Write-Host "Starting automated build process for cppio-rel..."

# Check if dev image exists, build if not
if (-not (Image-Exists $DEV_IMAGE_NAME)) {
    Write-Host "Image $DEV_IMAGE_NAME does not exist."
    if (-not (Build-CPPIO-Dev-Image)) {
        Write-Host "Failed to build image $DEV_IMAGE_NAME. Exiting."
        exit 1
    }
    exit 1
}

# Do CPPIO build in dev container
if (-not (Run-Build-In-Container)) {
    Write-Host "Build in container failed. Exiting."
    exit 1
}

# Run docker-compose build in WSL
if (-not (Run-Compose-Build-In-WSL)) {
    Write-Host "Docker-compose build in WSL $WSL_DISTRIBUTION_NAME failed. Exiting."
    exit 1
}

Write-Host "All steps completed successfully!"