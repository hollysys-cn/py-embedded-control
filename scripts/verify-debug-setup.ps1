#!/usr/bin/env pwsh
<#
.SYNOPSIS
    验证容器调试配置

.DESCRIPTION
    检查所有容器调试相关的配置文件是否存在且格式正确
#>

param()

$ErrorActionPreference = "Continue"

# 颜色输出
function Write-Status {
    param([string]$Message, [string]$Status)
    if ($Status -eq "OK") {
        Write-Host "✅ $Message" -ForegroundColor Green
    } elseif ($Status -eq "WARN") {
        Write-Host "⚠️  $Message" -ForegroundColor Yellow
    } else {
        Write-Host "❌ $Message" -ForegroundColor Red
    }
}

Write-Host "`n🔍 验证容器调试配置...`n" -ForegroundColor Cyan

# 1. 检查 Docker
Write-Host "1. Docker 环境检查" -ForegroundColor Yellow
try {
    $dockerVersion = docker --version
    Write-Status "Docker 已安装: $dockerVersion" "OK"
} catch {
    Write-Status "Docker 未安装或未运行" "ERROR"
}

# 2. 检查配置文件
Write-Host "`n2. 配置文件检查" -ForegroundColor Yellow

$configFiles = @(
    "Dockerfile",
    "docker-compose.yml",
    ".vscode\launch.json",
    ".vscode\tasks.json",
    ".vscode\settings.json",
    ".devcontainer\devcontainer.json",
    ".gitignore"
)

foreach ($file in $configFiles) {
    if (Test-Path $file) {
        Write-Status "配置文件存在: $file" "OK"
    } else {
        Write-Status "配置文件缺失: $file" "ERROR"
    }
}

# 3. 检查脚本文件
Write-Host "`n3. 脚本文件检查" -ForegroundColor Yellow

$scriptFiles = @(
    "debug-container.ps1",
    "debug-container.sh"
)

foreach ($file in $scriptFiles) {
    if (Test-Path $file) {
        Write-Status "脚本文件存在: $file" "OK"
    } else {
        Write-Status "脚本文件缺失: $file" "ERROR"
    }
}

# 4. 检查文档文件
Write-Host "`n4. 文档文件检查" -ForegroundColor Yellow

$docFiles = @(
    "docs\container-debug.md",
    "docs\container-debug-quickstart.md",
    "docs\container-debug-examples.md",
    "docs\CONTAINER_DEBUG_SETUP.md",
    "CONTAINER_DEBUG_SUMMARY.md"
)

foreach ($file in $docFiles) {
    if (Test-Path $file) {
        Write-Status "文档文件存在: $file" "OK"
    } else {
        Write-Status "文档文件缺失: $file" "ERROR"
    }
}

# 5. 检查 JSON 文件格式
Write-Host "`n5. JSON 格式检查" -ForegroundColor Yellow

$jsonFiles = @(
    ".vscode\launch.json",
    ".vscode\tasks.json",
    ".devcontainer\devcontainer.json"
)

foreach ($file in $jsonFiles) {
    if (Test-Path $file) {
        try {
            $null = Get-Content $file -Raw | ConvertFrom-Json
            Write-Status "JSON 格式正确: $file" "OK"
        } catch {
            Write-Status "JSON 格式错误: $file - $($_.Exception.Message)" "ERROR"
        }
    }
}

# 6. 检查 Docker Compose 配置
Write-Host "`n6. Docker Compose 配置检查" -ForegroundColor Yellow

try {
    $composeConfig = docker-compose config 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Status "docker-compose.yml 配置有效" "OK"

        # 检查服务
        if ($composeConfig -match "dev:") {
            Write-Status "服务 'dev' 已配置" "OK"
        }
        if ($composeConfig -match "arm-test:") {
            Write-Status "服务 'arm-test' 已配置" "OK"
        }

        # 检查端口
        if ($composeConfig -match "5678:5678") {
            Write-Status "Python 调试端口 5678 已配置" "OK"
        }
        if ($composeConfig -match "9000:9000") {
            Write-Status "C 调试端口 9000 已配置" "OK"
        }
    } else {
        Write-Status "docker-compose.yml 配置无效" "ERROR"
    }
} catch {
    Write-Status "无法验证 docker-compose 配置: $($_.Exception.Message)" "WARN"
}

# 7. 检查 VS Code 调试配置
Write-Host "`n7. VS Code 调试配置检查" -ForegroundColor Yellow

if (Test-Path ".vscode\launch.json") {
    $launchConfig = Get-Content ".vscode\launch.json" -Raw | ConvertFrom-Json

    $debugConfigs = @(
        "Python: 附加到 Docker 容器 (x86_64)",
        "Python: 附加到 Docker 容器 (ARM64)",
        "C++: 附加到容器中的进程 (gdbserver)",
        "混合调试: Python + C++"
    )

    foreach ($configName in $debugConfigs) {
        $found = $launchConfig.configurations.name -contains $configName
        if (-not $found -and $launchConfig.compounds) {
            $found = $launchConfig.compounds.name -contains $configName
        }

        if ($found) {
            Write-Status "调试配置存在: $configName" "OK"
        } else {
            Write-Status "调试配置缺失: $configName" "WARN"
        }
    }
}

# 8. 检查推荐的 VS Code 扩展
Write-Host "`n8. VS Code 扩展检查" -ForegroundColor Yellow

$recommendedExtensions = @(
    "ms-python.python",
    "ms-python.debugpy",
    "ms-vscode.cpptools",
    "ms-azuretools.vscode-docker"
)

if (Test-Path ".devcontainer\devcontainer.json") {
    $devcontainerConfig = Get-Content ".devcontainer\devcontainer.json" -Raw | ConvertFrom-Json

    if ($devcontainerConfig.customizations.vscode.extensions) {
        foreach ($ext in $recommendedExtensions) {
            if ($devcontainerConfig.customizations.vscode.extensions -contains $ext) {
                Write-Status "推荐扩展已配置: $ext" "OK"
            } else {
                Write-Status "推荐扩展未配置: $ext" "WARN"
            }
        }
    }
}

# 总结
Write-Host "`n" + "="*60 -ForegroundColor Cyan
Write-Host "验证完成！" -ForegroundColor Green
Write-Host "`n📚 下一步：" -ForegroundColor Cyan
Write-Host "  1. 阅读快速开始指南: docs\container-debug-quickstart.md"
Write-Host "  2. 启动容器: .\debug-container.ps1"
Write-Host "  3. 在 VS Code 中开始调试！"
Write-Host ""
