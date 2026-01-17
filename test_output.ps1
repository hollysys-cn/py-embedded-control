# 测试运行时输出
Write-Host "=== 测试运行时程序 ===" -ForegroundColor Cyan

Write-Host "`n1. 检查文件..." -ForegroundColor Yellow
docker exec plcopen-dev ls -l /workspace/bin/plcopen_runtime
docker exec plcopen-dev ls -l /workspace/config/pid_temperature.yaml
docker exec plcopen-dev ls -l /workspace/python/examples/pid_temperature.py

Write-Host "`n2. 查看配置文件..." -ForegroundColor Yellow
docker exec plcopen-dev cat /workspace/config/pid_temperature.yaml

Write-Host "`n3. 运行程序（按 Ctrl+C 停止）..." -ForegroundColor Yellow
docker exec -it plcopen-dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
