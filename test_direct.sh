#!/bin/bash
# 直接在容器内测试运行

echo "=== 测试 PLCopen 运行时 ==="
cd /workspace

echo ""
echo "1. 检查文件..."
ls -lh bin/plcopen_runtime config/pid_temperature.yaml

echo ""
echo "2. 测试 --help..."
./bin/plcopen_runtime --help

echo ""
echo "3. 创建日志目录..."
mkdir -p logs

echo ""
echo "4. 运行程序（3秒）..."
timeout 3s ./bin/plcopen_runtime --config config/pid_temperature.yaml 2>&1 || echo "程序退出码: $?"

echo ""
echo "5. 检查日志文件..."
if [ -f logs/pid_temperature.log ]; then
    echo "日志文件内容："
    head -50 logs/pid_temperature.log
else
    echo "日志文件不存在"
fi

echo ""
echo "=== 测试完成 ==="
