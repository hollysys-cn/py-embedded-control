#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Quick start script for container debugging

.DESCRIPTION
    Quickly start PLCopen Runtime dev container and launch debug server

.PARAMETER Platform
    Target platform: x86_64 or arm64 (default: x86_64)

.PARAMETER BuildImage
    Whether to rebuild the image (default: false)

.PARAMETER WaitDebugger
    Whether to wait for debugger connection (default: false)

.PARAMETER RunExample
    Example to run: pid_temperature, basic_pid, ramp_limit (default: none)

.EXAMPLE
    .\debug-container.ps1
    Start x86_64 container and keep it running

.EXAMPLE
    .\debug-container.ps1 -Platform arm64 -BuildImage
    Rebuild and start ARM64 container

.EXAMPLE
    .\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
    Start container, run PID temperature example, and wait for debugger
#>

param(
    [Parameter()]
    [ValidateSet("x86_64", "arm64")]
    [string]$Platform = "x86_64",

    [Parameter()]
    [switch]$BuildImage = $false,

    [Parameter()]
    [switch]$WaitDebugger = $false,

    [Parameter()]
    [ValidateSet("pid_temperature", "basic_pid", "ramp_limit", "")]
    [string]$RunExample = ""
)

# Error handling
$ErrorActionPreference = "Stop"

# Color output function
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

# Check if Docker is running
Write-ColorOutput "Checking Docker environment..." -Color Cyan
try {
    docker info | Out-Null
    Write-ColorOutput "SUCCESS: Docker is running" -Color Green
} catch {
    Write-ColorOutput "ERROR: Docker is not running. Please start Docker Desktop" -Color Red
    exit 1
}

# Determine service name and port
$ServiceName = if ($Platform -eq "arm64") { "arm-test" } else { "dev" }
$ContainerName = if ($Platform -eq "arm64") { "plcopen-arm-test" } else { "plcopen-dev" }
$DebugPort = if ($Platform -eq "arm64") { "5679" } else { "5678" }

Write-ColorOutput "" -Color White
Write-ColorOutput "Platform: $Platform" -Color Yellow
Write-ColorOutput "Service: $ServiceName" -Color Yellow
Write-ColorOutput "Container: $ContainerName" -Color Yellow
Write-ColorOutput "Debug Port: $DebugPort" -Color Yellow

# Build image if requested
if ($BuildImage) {
    Write-ColorOutput "" -Color White
    Write-ColorOutput "Building Docker image..." -Color Cyan
    docker-compose build $ServiceName
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput "ERROR: Image build failed" -Color Red
        exit 1
    }
    Write-ColorOutput "SUCCESS: Image built" -Color Green
}

# Stop and remove existing container
Write-ColorOutput "" -Color White
Write-ColorOutput "Cleaning up existing container..." -Color Cyan
docker-compose down $ServiceName 2>&1 | Out-Null

# Start container
Write-ColorOutput "" -Color White
Write-ColorOutput "Starting container..." -Color Cyan
docker-compose up -d $ServiceName
if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput "ERROR: Container startup failed" -Color Red
    exit 1
}

# Wait for container to be ready
Write-ColorOutput "Waiting for container to be ready..." -Color Cyan
Start-Sleep -Seconds 2

# Check container status
$ContainerStatus = docker inspect -f '{{.State.Running}}' $ContainerName 2>$null
if ($ContainerStatus -ne "true") {
    Write-ColorOutput "ERROR: Container is not running" -Color Red
    docker logs $ContainerName
    exit 1
}
Write-ColorOutput "SUCCESS: Container is running" -Color Green

# Display container info
Write-ColorOutput "" -Color White
Write-ColorOutput "Container information:" -Color Cyan
docker exec $ContainerName python3 --version
docker exec $ContainerName gcc --version | Select-Object -First 1

# Build project inside container
Write-ColorOutput "" -Color White
Write-ColorOutput "Building project inside container..." -Color Cyan
docker exec $ContainerName bash -c 'cd /workspace && python3 setup.py build_ext --inplace'
if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput "WARNING: Build failed, but continuing" -Color Yellow
}

docker exec $ContainerName bash -c 'cd /workspace && make runtime'
if ($LASTEXITCODE -ne 0) {
    Write-ColorOutput "WARNING: Runtime build failed, but continuing" -Color Yellow
}

# Run example (optional)
if ($RunExample) {
    Write-ColorOutput "" -Color White
    Write-ColorOutput "Preparing to run example: $RunExample" -Color Cyan

    $ExamplePath = "/workspace/python/examples/${RunExample}.py"
    $DebugArgs = if ($WaitDebugger) { "--wait-for-client" } else { "" }

    # Start debug server
    Write-ColorOutput "Starting debugpy server..." -Color Cyan
    $DebugCommand = "python3 -m debugpy --listen 0.0.0.0:5678 $DebugArgs $ExamplePath"

    Write-ColorOutput "" -Color White
    Write-ColorOutput "Executing command in container:" -Color Yellow
    Write-ColorOutput "  docker exec -it $ContainerName bash -c '$DebugCommand'" -Color Gray

    if ($WaitDebugger) {
        Write-ColorOutput "" -Color White
        Write-ColorOutput "Waiting for debugger to connect to localhost:$DebugPort" -Color Magenta
        Write-ColorOutput "  1. Press F5 in VS Code" -Color Gray
        Write-ColorOutput "  2. Select 'Python: Attach to Docker Container ($Platform)'" -Color Gray
        Write-ColorOutput "  3. The program will continue after debugger connects" -Color Gray
    }

    docker exec -it $ContainerName bash -c "$DebugCommand"
} else {
    Write-ColorOutput "" -Color White
    Write-ColorOutput "SUCCESS: Container is running and ready" -Color Green
    Write-ColorOutput "" -Color White
    Write-ColorOutput "Next steps:" -Color Cyan
    Write-ColorOutput "  1. Connect with VS Code 'Remote - Containers' extension" -Color Gray
    Write-ColorOutput "  2. Or enter container manually: docker exec -it $ContainerName bash" -Color Gray
    Write-ColorOutput "  3. Start debugging:" -Color Gray
    Write-ColorOutput "     - Python: python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py" -Color Gray
    Write-ColorOutput "     - C/C++: gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml" -Color Gray
    Write-ColorOutput "  4. Press F5 in VS Code to attach debugger" -Color Gray
    $GdbPort = if ($Platform -eq 'arm64') { '9001' } else { '9000' }
    Write-ColorOutput "" -Color White
    Write-ColorOutput "Debug ports: localhost:$DebugPort (Python), localhost:$GdbPort (gdbserver)" -Color Cyan
}

Write-ColorOutput "" -Color White
Write-ColorOutput "Container debug environment is ready!" -Color Green
