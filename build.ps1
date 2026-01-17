# PowerShell 构建脚本
# 用于在 Windows 环境中构建项目（使用 Docker）

param(
    [switch]$Runtime,
    [switch]$Python,
    [switch]$All,
    [switch]$Clean,
    [switch]$Test,
    [switch]$Lint
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
        Write-Host "请安装 Docker Desktop: https://www.docker.com/products/docker-desktop" -ForegroundColor Yellow
        return $false
    }
}

# 构建 Docker 镜像
function Build-DockerImage {
    Write-Info "构建 Docker 镜像..."
    docker-compose build dev
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Docker 镜像构建失败"
        exit 1
    }
    Write-Info "Docker 镜像构建成功"
}

# 构建运行时
function Build-Runtime {
    Write-Info "构建运行时程序..."
    docker-compose run --rm dev bash -c "make runtime"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "运行时构建失败"
        exit 1
    }
    Write-Info "运行时构建成功"
}

# 构建 Python 扩展
function Build-Python {
    Write-Info "构建 Python 扩展..."
    docker-compose run --rm dev bash -c "python3 setup.py build_ext --inplace"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Python 扩展构建失败"
        exit 1
    }
    Write-Info "Python 扩展构建成功"
}

# 清理构建产物
function Clean-Build {
    Write-Info "清理构建产物..."
    docker-compose run --rm dev bash -c "make clean"
    Write-Info "清理完成"
}

# 运行代码质量检查
function Run-Lint {
    Write-Info "运行代码质量检查..."
    docker-compose run --rm dev bash -c "make lint"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "代码质量检查发现问题"
        exit 1
    }
    Write-Info "代码质量检查通过"
}

# 运行测试
function Run-Tests {
    Write-Info "运行测试..."
    docker-compose run --rm dev bash -c "make test"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "测试失败"
        exit 1
    }
    Write-Info "测试通过"
}

# 主逻辑
if (-not (Test-Docker)) {
    exit 1
}

# 如果没有指定参数，显示帮助
if (-not ($Runtime -or $Python -or $All -or $Clean -or $Test -or $Lint)) {
    Write-Host @"
用法: .\build.ps1 [选项]

选项:
  -Runtime    构建 C 运行时程序
  -Python     构建 Python 扩展
  -All        构建所有（运行时 + Python 扩展）
  -Clean      清理构建产物
  -Lint       运行代码质量检查
  -Test       运行测试

示例:
  .\build.ps1 -All          # 构建所有
  .\build.ps1 -Runtime      # 仅构建运行时
  .\build.ps1 -Lint         # 代码质量检查
  .\build.ps1 -Clean        # 清理

注意: 需要 Docker Desktop 运行
"@
    exit 0
}

try {
    # 确保 Docker 镜像存在
    $imageExists = docker images plcopen-runtime-dev -q
    if (-not $imageExists) {
        Write-Info "Docker 镜像不存在，开始构建..."
        Build-DockerImage
    }

    if ($Clean) {
        Clean-Build
    }

    if ($All) {
        Build-Runtime
        Build-Python
    } else {
        if ($Runtime) {
            Build-Runtime
        }
        if ($Python) {
            Build-Python
        }
    }

    if ($Lint) {
        Run-Lint
    }

    if ($Test) {
        Run-Tests
    }

    Write-Info "所有操作完成"
} catch {
    Write-Error-Custom "构建过程中发生错误: $_"
    exit 1
}
