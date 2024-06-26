# PowerShell script to build, start, and remove the cppio_dev container
# Equivalent to startdevcontainer.sh

param(
    [Alias('b')][switch]$Build,
    [Alias('s')][switch]$Start,
    [Alias('r')][switch]$Remove,
    [Alias('p')][switch]$Proxy,
    [Alias('h')][switch]$Help,
    [Alias('ep')][switch]$SetExecutionPolicy,
    [Alias('l')][switch]$ListContainers
)
# Function to list containers (Docker or Podman)
function List-Containers {
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        Write-Host "Listing Docker containers (all):"
        docker ps -a
    } elseif (Get-Command podman -ErrorAction SilentlyContinue) {
        Write-Host "Listing Podman containers (all):"
        podman ps -a
    } else {
        Write-Host "Neither Docker nor Podman is installed."
    }
}
# Function to set PowerShell execution policy interactively
function Set-ExecutionPolicy-Interactive {
    $options = @('RemoteSigned', 'Bypass', 'Unrestricted', 'Restricted')
    Write-Host "Select execution policy to set for CurrentUser:"
    for ($i = 0; $i -lt $options.Count; $i++) {
        Write-Host "$($i+1). $($options[$i])"
    }
    $choice = Read-Host "Enter number (1-4) or policy name"
    $policy = $null
    if ($choice -match '^[1-4]$') {
        $policy = $options[[int]$choice-1]
    } elseif ($options -contains $choice) {
        $policy = $choice
    } else {
        Write-Host "Invalid choice. Aborting."
        return
    }
    try {
        Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy $policy -Force
        Write-Host "Execution policy set to $policy for CurrentUser."
    } catch {
        Write-Host "Failed to set execution policy: $_"
    }
}

function Set-Or-Remove-Proxy {
    # --- HTTP/HTTPS Proxy Check, Set, and Remove ---
    $proxyAction = $null
    if ((-not $env:HTTP_PROXY -and -not $env:http_proxy) -or (-not $env:HTTPS_PROXY -and -not $env:https_proxy)) {
        Write-Host "No HTTP or HTTPS proxy is set in the environment."
        $proxyAction = Read-Host "Do you want to set HTTP/HTTPS proxy for this session? (y=set, r=remove, n=skip)"
    } else {
        $currentHttpProxy = if ($env:HTTP_PROXY) { $env:HTTP_PROXY } elseif ($env:http_proxy) { $env:http_proxy } else { '' }
        $currentHttpsProxy = if ($env:HTTPS_PROXY) { $env:HTTPS_PROXY } elseif ($env:https_proxy) { $env:https_proxy } else { '' }
        Write-Host "Current HTTP proxy: $currentHttpProxy"
        Write-Host "Current HTTPS proxy: $currentHttpsProxy"
        $proxyAction = Read-Host "Do you want to set or remove HTTP/HTTPS proxy for this session? (y=set, r=remove, n=skip)"
    }

    $noProxyList = "localhost,127.0.0.1,::1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16,fc00::/7,*.local"

    if ($proxyAction -eq 'y') {
        $proxyUrl = Read-Host "Enter HTTP proxy URL (e.g., http://proxy.example.com:8080, leave blank to skip)"
        if ($proxyUrl) {
            $env:HTTP_PROXY = $proxyUrl
            $env:http_proxy = $proxyUrl
            $env:NO_PROXY = $noProxyList
            $env:no_proxy = $noProxyList
            Write-Host "HTTP proxy set for this session: $proxyUrl"
            Write-Host "NO_PROXY set for local and private network addresses."
        }
        $proxyUrlS = Read-Host "Enter HTTPS proxy URL (e.g., https://proxy.example.com:8443, leave blank to skip)"
        if ($proxyUrlS) {
            $env:HTTPS_PROXY = $proxyUrlS
            $env:https_proxy = $proxyUrlS
            $env:NO_PROXY = $noProxyList
            $env:no_proxy = $noProxyList
            Write-Host "HTTPS proxy set for this session: $proxyUrlS"
            Write-Host "NO_PROXY set for local and private network addresses."
        }
    } elseif ($proxyAction -eq 'r') {
        Remove-Item Env:HTTP_PROXY -ErrorAction SilentlyContinue
        Remove-Item Env:http_proxy -ErrorAction SilentlyContinue
        Remove-Item Env:HTTPS_PROXY -ErrorAction SilentlyContinue
        Remove-Item Env:https_proxy -ErrorAction SilentlyContinue
        Remove-Item Env:NO_PROXY -ErrorAction SilentlyContinue
        Remove-Item Env:no_proxy -ErrorAction SilentlyContinue
        Write-Host "HTTP, HTTPS, and NO_PROXY environment variables have been removed for this session."
    }
    # -------------------------------
}

# --- Configuration ---
$IMAGE_NAME = "cppio_dev"
$DOCKERFILE_PATH = Split-Path -Parent (Resolve-Path $MyInvocation.MyCommand.Path)
Write-Host "The absolute current directory (using Resolve-Path) is: $DOCKERFILE_PATH"
# ---------------------

function Build-With-Docker {
    Write-Host "Attempting to build with Docker..."
    $dockerfile = "Dockerfile-$(Get-ComputerInfo -Property OsArchitecture | ForEach-Object { if ($_ -like '*64*') { 'x86_64' } else { 'arm64' } })"
    $cmd = "docker build -t $IMAGE_NAME -f $DOCKERFILE_PATH/$dockerfile $DOCKERFILE_PATH"
    Write-Host "Command: $cmd"
    Invoke-Expression $cmd
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Docker build successful!"
        return $true
    } else {
        Write-Host "Docker build failed."
        return $false
    }
}

function Build-With-Podman {
    Write-Host "Attempting to build with Podman..."
    $dockerfile = "Dockerfile-$(Get-ComputerInfo -Property OsArchitecture | ForEach-Object { if ($_ -like '*64*') { 'x86_64' } else { 'arm64' } })"
    $cmd = "podman build --network=host -t $IMAGE_NAME -f $DOCKERFILE_PATH/$dockerfile $DOCKERFILE_PATH"
    Write-Host "Command: $cmd"
    Invoke-Expression $cmd
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Podman build successful!"
        return $true
    } else {
        Write-Host "Podman build failed."
        return $false
    }
}

function Build-Image {
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        if (-not (Build-With-Docker)) {
            Write-Host "Docker build failed. Trying Podman..."
            if (Get-Command podman -ErrorAction SilentlyContinue) {
                if (-not (Build-With-Podman)) {
                    Write-Host "Podman build failed. Exiting."
                    exit 1
                }
            } else {
                Write-Host "Podman not found. Please install Docker or Podman to build the image."
                exit 1
            }
        }
    } elseif (Get-Command podman -ErrorAction SilentlyContinue) {
        if (-not (Build-With-Podman)) {
            Write-Host "Podman build failed. Exiting."
            exit 1
        }
    } else {
        Write-Host "Docker and Podman not found. Please install either Docker or Podman to build the image."
        exit 1
    }
}

function Start-Container {
    $ct_run_parm = "--network=host -v $($DOCKERFILE_PATH)/..:/workspace/cppio -it --name $IMAGE_NAME ${IMAGE_NAME}:latest"
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        $docker_run_cmd = "docker run --privileged $ct_run_parm"
        Write-Host "Start container $IMAGE_NAME by Docker"
        Write-Host "Command: $docker_run_cmd"
        Invoke-Expression $docker_run_cmd
    } elseif (Get-Command podman -ErrorAction SilentlyContinue) {
        Write-Host "Start container $IMAGE_NAME by Podman"
        Write-Host "Command: podman run $ct_run_parm"
        Invoke-Expression "podman run $ct_run_parm"
    } else {
        Write-Host "Neither Docker nor Podman found."
        exit 1
    }
}

function Remove-Container {
    if (Get-Command docker -ErrorAction SilentlyContinue) {
        Write-Host "Remove container $IMAGE_NAME by Docker"
        Write-Host "Command: docker rm $IMAGE_NAME --force"
        Invoke-Expression "docker rm $IMAGE_NAME --force"
    } elseif (Get-Command podman -ErrorAction SilentlyContinue) {
        Write-Host "Remove container $IMAGE_NAME by Podman"
        Write-Host "Command: podman rm $IMAGE_NAME --force"
        Invoke-Expression "podman rm $IMAGE_NAME --force"
    } else {
        Write-Host "Neither Docker nor Podman found."
        exit 1
    }
}

if ($SetExecutionPolicy) {
    Set-ExecutionPolicy-Interactive
}

if ($ListContainers) {
    List-Containers
}

if ($Proxy) {
    Set-Or-Remove-Proxy
}

if ($Help) {
    Write-Host "Usage: .\\startdevcontainer.ps1 [-Build] [-Start] [-Remove] [-Proxy] [-SetExecutionPolicy] [-ListContainers] [-Help]"
    Write-Host "  -Build (-b): Build the container image."
    Write-Host "  -Start (-s): Start the container."
    Write-Host "  -Remove (-r): Remove the container."
    Write-Host "  -Proxy (-p): Set or remove HTTP/HTTPS proxy for this session."
    Write-Host "  -SetExecutionPolicy (-ep): Interactively set PowerShell execution policy for CurrentUser (RemoteSigned, Bypass, Unrestricted, Restricted)."
    Write-Host "  -ListContainers (-l): List all containers (Docker or Podman)."
    Write-Host "  -Help (-h): Show this help message."
    exit 0
}

if ($Build) {
    Write-Host "Build Image..."
    Build-Image
}

if ($Remove) {
    Write-Host "Remove Container..."
    Remove-Container
}

if ($Start) {
    Write-Host "Start Container..."
    Start-Container
}
