# PowerShell script to log into the cppio_dev container (Docker or Podman)

# --- Configuration ---
$IMAGE_NAME = "cppio_dev"
$DOCKERFILE_PATH = Split-Path -Parent (Resolve-Path $MyInvocation.MyCommand.Path)
Write-Host "The absolute current directory (using Resolve-Path) is: $DOCKERFILE_PATH"
# ---------------------

if (Get-Command docker -ErrorAction SilentlyContinue) {
    Write-Host "Log into container $IMAGE_NAME by Docker"
    docker container start $IMAGE_NAME | Out-Null
    Write-Host "Command: docker exec -it $IMAGE_NAME /bin/bash"
    docker exec -it $IMAGE_NAME /bin/bash
} elseif (Get-Command podman -ErrorAction SilentlyContinue) {
    Write-Host "Log into container $IMAGE_NAME by Podman"
    podman container start $IMAGE_NAME | Out-Null
    Write-Host "Command: podman exec -it $IMAGE_NAME /bin/bash"
    podman exec -it $IMAGE_NAME /bin/bash
} else {
    Write-Host "Neither Docker nor Podman found."
    exit 1
}
