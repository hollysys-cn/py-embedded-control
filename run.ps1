# PowerShell 运行脚本
# 用于在 Windows 环境中运行项目（使用 Docker）

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

# 检查 Docker 是否可用
function Test-Docker {
    try {
        $null = docker --version
        return $true
    } catch {
        Write-Error-Custom "Docker 未安装或未运行"
        return $false
    }
}

# 主逻辑
if (-not (Test-Docker)) {
    exit 1
}

if ($Shell) {
    Write-Info "启动 Docker 开发环境 Shell..."
    docker-compose run --rm dev bash
    exit 0
}

if ($Debug) {
    $Config = "config/pid_temperature_debug.yaml"
    Write-Info "使用调试配置: $Config"
    Write-Info "调试端口: 5678"
    Write-Info "VS Code: 按 F5 选择 'Python: 附加到运行时'"
}

Write-Info "启动运行时: $Config"
Write-Info "按 Ctrl+C 停止运行"

docker-compose run --rm --service-ports dev bash -c "./bin/plcopen_runtime --config $Config"
