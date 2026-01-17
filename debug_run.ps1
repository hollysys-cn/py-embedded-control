# 调试运行脚本 - 查找为什么没有输出
Write-Host "=== 调试 PLCopen 运行时 ===" -ForegroundColor Cyan

# 1. 进入容器执行测试
Write-Host "`n1. 在容器内运行测试脚本..." -ForegroundColor Yellow
$output = docker exec plcopen-dev bash -c @"
cd /workspace
echo '--- 文件检查 ---'
ls -lh bin/plcopen_runtime config/pid_temperature.yaml

echo ''
echo '--- 测试帮助信息 ---'
./bin/plcopen_runtime --help

echo ''
echo '--- 运行程序 3 秒 ---'
mkdir -p logs
timeout 3s ./bin/plcopen_runtime --config config/pid_temperature.yaml 2>&1 || true

echo ''
echo '--- 检查日志 ---'
if [ -f logs/pid_temperature.log ]; then
  head -30 logs/pid_temperature.log
else
  echo '日志文件不存在'
fi
"@

Write-Host $output

Write-Host "`n=== 调试完成 ===" -ForegroundColor Cyan
