# 测试运行脚本
Write-Host "=== 测试 PLCopen 运行时 ===" -ForegroundColor Cyan

# 1. 检查容器状态
Write-Host "`n1. 检查容器状态..." -ForegroundColor Yellow
docker ps -a | Select-String "plcopen"

# 2. 启动容器（如果未运行）
Write-Host "`n2. 确保容器运行..." -ForegroundColor Yellow
docker-compose up -d dev
Start-Sleep -Seconds 2

# 3. 检查文件
Write-Host "`n3. 检查关键文件..." -ForegroundColor Yellow
docker exec plcopen-dev bash -c "ls -l /workspace/bin/plcopen_runtime /workspace/config/pid_temperature.yaml /workspace/python/examples/pid_temperature.py"

# 4. 测试运行时程序（3秒）
Write-Host "`n4. 运行程序（3秒测试）..." -ForegroundColor Yellow
Write-Host "预期：应该看到Python输出" -ForegroundColor Gray
$job = Start-Job -ScriptBlock {
    docker exec plcopen-dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
}
Start-Sleep -Seconds 3
Stop-Job $job
$output = Receive-Job $job
Remove-Job $job

if ($output) {
    Write-Host "✓ 输出内容：" -ForegroundColor Green
    $output
} else {
    Write-Host "✗ 没有输出！" -ForegroundColor Red
}

# 5. 检查日志
Write-Host "`n5. 检查日志目录..." -ForegroundColor Yellow
docker exec plcopen-dev bash -c "ls -la /workspace/logs/ 2>/dev/null || echo '日志目录不存在'"

Write-Host "`n=== 测试完成 ===" -ForegroundColor Cyan
