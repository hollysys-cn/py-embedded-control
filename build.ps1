# PowerShell Build Script
# Build PLCopen Python Runtime on Windows using Docker

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

# Check if Docker is available
function Test-Docker {
    try {
        $null = docker --version
        return $true
    } catch {
        Write-Error-Custom "Docker is not installed or not running"
        Write-Host "Please install Docker Desktop: https://www.docker.com/products/docker-desktop" -ForegroundColor Yellow
        return $false
    }
}

# Build Docker image
function Build-DockerImage {
    Write-Info "Building Docker image..."
    docker-compose build dev
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Docker image build failed"
        exit 1
    }
    Write-Info "Docker image built successfully"
}

# Build runtime
function Build-Runtime {
    Write-Info "Building runtime..."
    docker-compose run --rm dev bash -c "cd /workspace && mkdir -p bin && make runtime"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Runtime build failed"
        exit 1
    }
    Write-Info "Runtime built successfully"
}

# Build Python extension
function Build-Python {
    Write-Info "Building Python extension..."
    docker-compose run --rm dev bash -c "cd /workspace && python3 setup.py build_ext --inplace"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Python extension build failed"
        exit 1
    }
    Write-Info "Python extension built successfully"
}

# Clean build artifacts
function Clean-Build {
    Write-Info "Cleaning build artifacts..."
    docker-compose run --rm dev bash -c "cd /workspace && make clean"
    Write-Info "Clean completed"
}

# Run code quality checks
function Run-Lint {
    Write-Info "Running code quality checks..."
    docker-compose run --rm dev bash -c "cd /workspace && make lint"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Code quality issues found"
        exit 1
    }
    Write-Info "Code quality checks passed"
}

# Run tests
function Run-Tests {
    Write-Info "Running tests..."
    docker-compose run --rm dev bash -c "cd /workspace && python3 test_module.py"
    if ($LASTEXITCODE -ne 0) {
        Write-Error-Custom "Tests failed"
        exit 1
    }
    Write-Info "Tests passed"
}

# Main logic
if (-not (Test-Docker)) {
    exit 1
}

# Show help if no parameters specified
if (-not ($Runtime -or $Python -or $All -or $Clean -or $Test -or $Lint)) {
    Write-Host @"
Usage: .\build.ps1 [options]

Options:
  -Runtime    Build C runtime
  -Python     Build Python extension
  -All        Build all (runtime + Python extension)
  -Clean      Clean build artifacts
  -Lint       Run code quality checks
  -Test       Run tests

Examples:
  .\build.ps1 -All          # Build everything
  .\build.ps1 -Runtime      # Build runtime only
  .\build.ps1 -Lint         # Run code checks
  .\build.ps1 -Clean        # Clean build

Note: Docker Desktop must be running
"@
    exit 0
}

try {
    # Ensure Docker image exists
    $imageExists = docker images plcopen-runtime-dev -q
    if (-not $imageExists) {
        Write-Info "Docker image does not exist, building..."
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

    Write-Info "All operations completed successfully"
} catch {
    Write-Error-Custom "Build error: $_"
    exit 1
}
