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

    # 确保容器正在运行
    $containerStatus = docker ps -q -f name=plcopen-dev
    if (-not $containerStatus) {
        Write-Info "Starting development container..."
        docker-compose up -d dev
        Start-Sleep -Seconds 2
    }

    docker exec -it plcopen-dev bash
    exit 0
}

if ($Debug) {
    $Config = "config/pid_temperature_debug.yaml"
    Write-Info "Using debug configuration: $Config"
    Write-Info "Debug port: 5678"
    Write-Info "VS Code: Press F5 and select 'Python: Attach to Runtime'"
}

# 确保开发容器正在运行
$containerStatus = docker ps -q -f name=plcopen-dev
if (-not $containerStatus) {
    Write-Info "Starting development container..."
    docker-compose up -d dev
    Start-Sleep -Seconds 2
}

Write-Info "Starting runtime: $Config"
Write-Info "Press Ctrl+C to stop"
Write-Info "Note: Python output will be shown in real-time"

# 使用 docker exec 在运行中的容器内执行命令
# 这样 Ctrl+C 可以正常工作
docker exec -it plcopen-dev bash -c "cd /workspace && ./bin/plcopen_runtime --config $Config"
