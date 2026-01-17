#!/bin/bash
# 使用 Valgrind 进行详细的内存泄漏检测

RUNTIME_BIN="./bin/plcopen_runtime"
CONFIG_FILE="./config/pid_temperature.yaml"
VALGRIND_LOG="logs/valgrind_$(date +%Y%m%d_%H%M%S).log"
TEST_DURATION_SEC=300  # 5 分钟快速测试

echo "========================================"
echo "Valgrind 内存泄漏检测"
echo "========================================"
echo ""

# 检查 Valgrind 是否安装
if ! command -v valgrind &> /dev/null; then
    echo "错误：Valgrind 未安装"
    echo "安装方法："
    echo "  Debian/Ubuntu: sudo apt-get install valgrind"
    echo "  CentOS/RHEL: sudo yum install valgrind"
    exit 1
fi

# 检查运行时程序
if [ ! -f "$RUNTIME_BIN" ]; then
    echo "错误：运行时程序不存在: $RUNTIME_BIN"
    exit 1
fi

# 创建日志目录
mkdir -p logs

echo "使用 Valgrind 运行程序（运行 $TEST_DURATION_SEC 秒）..."
echo "日志文件: $VALGRIND_LOG"
echo ""

# 运行 Valgrind
timeout $TEST_DURATION_SEC valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=$VALGRIND_LOG \
    $RUNTIME_BIN --config $CONFIG_FILE

EXIT_CODE=$?

# 检查退出代码
if [ $EXIT_CODE -eq 124 ]; then
    echo "程序已运行 $TEST_DURATION_SEC 秒后被 timeout 终止（正常）"
elif [ $EXIT_CODE -ne 0 ]; then
    echo "警告：程序非正常退出，退出代码: $EXIT_CODE"
fi

echo ""
echo "分析 Valgrind 报告..."
echo "========================================"

# 提取关键信息
echo ""
echo "1. 内存泄漏总结"
echo "----------------------------------------"
grep "LEAK SUMMARY" -A 5 $VALGRIND_LOG | head -6

echo ""
echo "2. 确定丢失的内存"
echo "----------------------------------------"
grep "definitely lost:" $VALGRIND_LOG

echo ""
echo "3. 间接丢失的内存"
echo "----------------------------------------"
grep "indirectly lost:" $VALGRIND_LOG

echo ""
echo "4. 可能丢失的内存"
echo "----------------------------------------"
grep "possibly lost:" $VALGRIND_LOG

echo ""
echo "========================================"
echo "完整报告: $VALGRIND_LOG"
echo ""

# 分析结果
DEFINITELY_LOST=$(grep "definitely lost:" $VALGRIND_LOG | awk '{print $4}' | sed 's/,//g')

if [ -z "$DEFINITELY_LOST" ] || [ "$DEFINITELY_LOST" = "0" ]; then
    echo "✓ 测试通过：未检测到确定的内存泄漏"
    exit 0
else
    echo "✗ 测试失败：检测到 $DEFINITELY_LOST 字节的确定内存泄漏"
    echo "请查看详细报告: $VALGRIND_LOG"
    exit 1
fi
