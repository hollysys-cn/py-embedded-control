# PowerShell Run Script
# Run PLCopen Python Runtime on Windows using Docker

param(
    [string]$Config = "config/pid_temperature.yaml",
    [switch]$Debug,
    [switch]$Shell
)

$ErrorActionPreference = "Stop"

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Green
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

# Check if Docker is available
function Test-Docker {
    try {
        $null = docker --version
        return $true
    } catch {
        Write-Error-Custom "Docker is not installed or not running"
        return $false
    }
}

# Main logic
if (-not (Test-Docker)) {
    exit 1
}

if ($Shell) {
    Write-Info "Starting Docker development shell..."
    docker-compose run --rm dev bash
    exit 0
}

if ($Debug) {
    $Config = "config/pid_temperature_debug.yaml"
    Write-Info "Using debug configuration: $Config"
    Write-Info "Debug port: 5678"
    Write-Info "VS Code: Press F5 and select 'Python: Attach to Runtime'"
}

Write-Info "Starting runtime: $Config"
Write-Info "Press Ctrl+C to stop"

docker-compose run --rm --service-ports dev bash -c "cd /workspace && ./bin/plcopen_runtime --config $Config"
