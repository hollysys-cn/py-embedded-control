#!/usr/bin/env pwsh
# PLCopen Python 运行时 - 自动启动远程调试
# 用途：一键启动 Docker 容器、调试服务器并打开 VS Code 调试会话

param(
    [switch]$Rebuild,      # 重新构建容器
    [switch]$NoWait,       # 不等待调试客户端连接
    [string]$Example = "pid_temperature"  # 要运行的示例脚本
)

$ErrorActionPreference = "Stop"

Write-Host "=== PLCopen Python 运行时 - 远程调试自动启动 ===" -ForegroundColor Cyan
Write-Host ""

# 步骤 1：检查 Docker 是否运行
Write-Host "[1/5] 检查 Docker 服务..." -ForegroundColor Yellow
try {
    docker ps | Out-Null
    Write-Host "  ✓ Docker 服务正常运行" -ForegroundColor Green
} catch {
    Write-Host "  ✗ Docker 服务未运行，请先启动 Docker Desktop" -ForegroundColor Red
    exit 1
}

# 步骤 2：启动或重启容器
Write-Host "[2/5] 准备 Docker 容器..." -ForegroundColor Yellow

$containerRunning = docker ps --filter "name=plcopen-dev" --format "{{.Names}}" 2>$null
if ($containerRunning -eq "plcopen-dev") {
    if ($Rebuild) {
        Write-Host "  停止现有容器..." -ForegroundColor Gray
        docker-compose down dev 2>&1 | Out-Null
        Write-Host "  重新构建并启动容器..." -ForegroundColor Gray
        docker-compose up -d --build dev
    } else {
        Write-Host "  ✓ 容器已运行，跳过启动" -ForegroundColor Green
    }
} else {
    Write-Host "  启动开发容器..." -ForegroundColor Gray
    if ($Rebuild) {
        docker-compose up -d --build dev
    } else {
        docker-compose up -d dev
    }
}

# 等待容器完全启动
Start-Sleep -Seconds 2

# 验证容器状态
$containerStatus = docker inspect plcopen-dev --format "{{.State.Running}}" 2>$null
if ($containerStatus -ne "true") {
    Write-Host "  ✗ 容器启动失败" -ForegroundColor Red
    docker logs plcopen-dev --tail 50
    exit 1
}

Write-Host "  ✓ 容器运行正常" -ForegroundColor Green
Write-Host "    容器 ID: $(docker ps --filter "name=plcopen-dev" --format "{{.ID}}")" -ForegroundColor Gray
Write-Host "    端口映射: 5678:5678 (debugpy), 9000:9000 (gdbserver)" -ForegroundColor Gray

# 步骤 3：构建项目（如果需要）
Write-Host "[3/5] 构建项目..." -ForegroundColor Yellow
$buildOutput = docker exec plcopen-dev bash -c "cd /workspace && python3 setup.py build_ext --inplace 2>&1"
if ($LASTEXITCODE -ne 0) {
    Write-Host "  ⚠ 构建警告（可能已构建）" -ForegroundColor Yellow
    Write-Host "    $buildOutput" -ForegroundColor Gray
} else {
    Write-Host "  ✓ 项目构建成功" -ForegroundColor Green
}

# 步骤 4：启动调试服务器
Write-Host "[4/5] 启动 debugpy 服务器..." -ForegroundColor Yellow

# 停止可能存在的旧调试进程
docker exec plcopen-dev pkill -f "debugpy" 2>$null | Out-Null

# 确定示例脚本路径
$scriptPath = "python/examples/$Example.py"
Write-Host "  示例脚本: $scriptPath" -ForegroundColor Gray

# 构建启动命令
$debugCmd = "cd /workspace && python3 -Xfrozen_modules=off -m debugpy --listen 0.0.0.0:5678"
if (-not $NoWait) {
    $debugCmd += " --wait-for-client"
}
$debugCmd += " $scriptPath"

# 在后台启动调试服务器
Write-Host "  启动命令: python3 -m debugpy --listen 0.0.0.0:5678 $(if(-not $NoWait){'--wait-for-client'}) $scriptPath" -ForegroundColor Gray
$null = Start-Job -ScriptBlock {
    param($debugCmd)
    docker exec -i plcopen-dev bash -c $debugCmd
} -ArgumentList $debugCmd

# 等待调试服务器启动
Start-Sleep -Seconds 2

# 验证调试服务器
$debugPid = docker exec plcopen-dev pgrep -f "debugpy" 2>$null
if (-not $debugPid) {
    Write-Host "  ✗ debugpy 服务器启动失败" -ForegroundColor Red
    Get-Job | Receive-Job
    exit 1
}

Write-Host "  ✓ debugpy 服务器运行中 (PID: $debugPid)" -ForegroundColor Green
Write-Host "    监听地址: 0.0.0.0:5678" -ForegroundColor Gray
if (-not $NoWait) {
    Write-Host "    等待客户端连接..." -ForegroundColor Gray
}

# 步骤 5：提示用户附加调试器
Write-Host "[5/5] 准备附加调试器..." -ForegroundColor Yellow
Write-Host ""
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host "  调试环境已就绪！" -ForegroundColor Green
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""
Write-Host "下一步：在 VS Code 中附加调试器" -ForegroundColor Yellow
Write-Host ""
Write-Host "  1. 按 Ctrl+Shift+D 打开'运行和调试'视图" -ForegroundColor White
Write-Host "  2. 选择: 'Python: 附加到 Docker 容器 (x86_64)'" -ForegroundColor White
Write-Host "  3. 按 F5 开始调试" -ForegroundColor White
Write-Host ""
Write-Host "或者运行以下命令手动附加：" -ForegroundColor Gray
Write-Host "  code --command workbench.action.debug.start" -ForegroundColor Cyan
Write-Host ""
Write-Host "调试技巧：" -ForegroundColor Yellow
Write-Host "  • 在 python/examples/$Example.py 中设置断点" -ForegroundColor White
Write-Host "  • 使用 F10 单步执行，F11 步入函数" -ForegroundColor White
Write-Host "  • 在'变量'面板查看实时数据" -ForegroundColor White
Write-Host "  • 在'调试控制台'评估表达式" -ForegroundColor White
Write-Host ""
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

# 提供快速命令参考
Write-Host "常用命令：" -ForegroundColor Yellow
Write-Host "  查看容器日志:  docker logs -f plcopen-dev" -ForegroundColor Gray
Write-Host "  进入容器 Shell: docker exec -it plcopen-dev bash" -ForegroundColor Gray
Write-Host "  停止调试:      docker exec plcopen-dev pkill -f debugpy" -ForegroundColor Gray
Write-Host "  停止容器:      docker-compose down" -ForegroundColor Gray
Write-Host ""

# 可选：自动打开 VS Code
$openVSCode = Read-Host "是否自动打开 VS Code 并启动调试？ (y/N)"
if ($openVSCode -eq 'y' -or $openVSCode -eq 'Y') {
    Write-Host "  正在启动 VS Code 调试会话..." -ForegroundColor Gray
    Start-Sleep -Seconds 1
    code --command workbench.action.debug.start
    Write-Host "  ✓ VS Code 调试会话已启动" -ForegroundColor Green
} else {
    Write-Host "  跳过自动启动，请手动在 VS Code 中附加调试器" -ForegroundColor Gray
}

Write-Host ""
Write-Host "脚本执行完成！按任意键退出..." -ForegroundColor Cyan
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# 清理后台任务
Get-Job | Remove-Job -Force
