# Copyright 2026 cppio authors. All rights reserved.
# PowerShell script to automate building cppio-rel for Docker Compose on Windows 11
# This script builds the project in the dev container, then runs docker-compose build in WSL

param(
    [Alias('d')][string]$DevImageName = 'cppio/cppio-dev',
    [Alias('n')][string]$ImageName = 'cppio/cppio-rel',
    [Alias('e')][string]$Engine = 'docker',
    [Alias('s')][int]$Step = 2,
    [Alias('m')][string]$Make = 'cppio',
    [Alias('w')][string]$WslDis = 'Ubuntu-24.04',
    [Alias('h')][switch]$Help
)

# --- Configuration ---
$CPPIO_PROJECT_NAME = "cppio"
$BUILD_CONTAINER_NAME = "cppio_dev_build_temp"

$CPPIO_ROOT = Split-Path -Parent (Resolve-Path $MyInvocation.MyCommand.Path)
$CPPIO_DEPLOY_ROOT = Join-Path $CPPIO_ROOT "deploy"
$DOCKERFILE_PATH = Join-Path $CPPIO_DEPLOY_ROOT "SuseContainerImageFile"
$WORKSPACE_PATH = $CPPIO_ROOT
# ---------------------

if ($Help) {
    Write-Host "Usage: .\\build-cppio-image.ps1 [-Engine {docker|podman}] [-Help]"
    Write-Host "This script automates the build process for CPPIO image ${ImageName}:"
    Write-Host "1. Checks if the CPPIO dev image $DevImageName exists"
    Write-Host "2. Builds CPPIO project inside a tmp dev container (image $DevImageName)"
    Write-Host "3. Runs docker-compose build cppio-hsd in WSL ${WslDis}"
    Write-Host ""
    Write-Host "OPTIONS:"
    Write-Host "  -DevImageName (-d)    Specify image name for CPPIO dev (default: cppio/cppio-dev)."
    Write-Host "  -ImageName    (-n)    Specify image name for CPPIO (default: cppio/cppio-rel)."
    Write-Host "  -Engine       (-e)    Specify container engine: docker or podman (default: docker)."
    Write-Host "  -Step         (-s)    1 for dev image build, 2 for all."
    Write-Host "  -Make         (-m)    all to build all source codes, cppio for only cppio source codes."
    Write-Host "  -WslDis       (-e)    Specify WSL distribution (default: Ubuntu-24.04)."
    Write-Host "  -Help         (-h)    Show this help message."
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
    Write-Host "Building CPPIO dev image $DevImageName..."
    $devContainerPath = Join-Path -Path $CPPIO_ROOT -ChildPath "devcontainer"
    $startDevScript = Join-Path -Path $devContainerPath -ChildPath "startdevcontainer.ps1"
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
    $build_all_thirds = if ($Make -eq "all") { "&& ./bootstrap.sh all" } else { "" }
    $CPPIO_BUILD_COMMANDS = "cd /workspace/cppio ${build_all_thirds} && ./makecppio.sh"
    $cmd = "$Engine run --rm --name $BUILD_CONTAINER_NAME -v ${WORKSPACE_PATH}:/workspace/cppio $DevImageName /bin/bash -c `"${CPPIO_BUILD_COMMANDS}`""
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
    $wslCmd = "wsl -d $WslDis -e bash -c `"${composeCmd}`""
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
Write-Host "Starting automated build process for ${ImageName}..."

# Check if dev image exists, build if not
if (-not (Image-Exists $DevImageName)) {
    Write-Host "Image $DevImageName does not exist."
    if (-not (Build-CPPIO-Dev-Image)) {
        Write-Host "Failed to build image $DevImageName. Exiting."
        exit 1
    }
    exit 1
}

if ($Step -eq 1) {
    exit 0
}

# Do CPPIO build in dev container
if (-not (Run-Build-In-Container)) {
    Write-Host "Build in container failed. Exiting."
    exit 1
}

# Run docker-compose build in WSL
if (-not (Run-Compose-Build-In-WSL)) {
    Write-Host "Docker-compose build in WSL $WslDis failed. Exiting."
    exit 1
}

Write-Host "All steps completed successfully!"