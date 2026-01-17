#!/bin/bash
# 内存泄漏检测脚本
# 运行运行时环境并监控内存使用情况

RUNTIME_BIN="./bin/plcopen_runtime"
CONFIG_FILE="./config/pid_temperature.yaml"
TEST_DURATION_HOURS=24
SAMPLE_INTERVAL_SEC=60
LOG_FILE="logs/memory_leak_test_$(date +%Y%m%d_%H%M%S).log"

echo "========================================"
echo "内存泄漏检测测试"
echo "========================================"
echo "运行时间: $TEST_DURATION_HOURS 小时"
echo "采样间隔: $SAMPLE_INTERVAL_SEC 秒"
echo "日志文件: $LOG_FILE"
echo ""

# 检查运行时程序是否存在
if [ ! -f "$RUNTIME_BIN" ]; then
    echo "错误：运行时程序不存在: $RUNTIME_BIN"
    echo "请先运行: make runtime"
    exit 1
fi

# 检查配置文件是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    echo "错误：配置文件不存在: $CONFIG_FILE"
    exit 1
fi

# 创建日志目录
mkdir -p logs

# 启动运行时程序（后台运行）
echo "启动运行时程序..."
$RUNTIME_BIN --config $CONFIG_FILE > logs/runtime_output.log 2>&1 &
RUNTIME_PID=$!

# 检查进程是否成功启动
sleep 2
if ! kill -0 $RUNTIME_PID 2>/dev/null; then
    echo "错误：运行时程序启动失败"
    cat logs/runtime_output.log
    exit 1
fi

echo "运行时程序已启动，PID: $RUNTIME_PID"
echo ""

# 初始内存读取
INITIAL_MEM=$(ps -o rss= -p $RUNTIME_PID)
if [ -z "$INITIAL_MEM" ]; then
    echo "错误：无法读取进程内存"
    kill $RUNTIME_PID
    exit 1
fi

echo "初始内存使用: $INITIAL_MEM KB"
echo ""

# 写入日志头
{
    echo "========================================"
    echo "内存泄漏检测测试"
    echo "========================================"
    echo "开始时间: $(date)"
    echo "PID: $RUNTIME_PID"
    echo "初始内存: $INITIAL_MEM KB"
    echo "测试时长: $TEST_DURATION_HOURS 小时"
    echo ""
    echo "时间,内存(KB),增长(KB),增长率(%)"
} > $LOG_FILE

# 监控循环
TOTAL_SAMPLES=$((TEST_DURATION_HOURS * 3600 / SAMPLE_INTERVAL_SEC))
SAMPLE_COUNT=0

echo "开始监控... (按 Ctrl+C 停止)"
echo ""

trap "echo ''; echo '测试被中断'; kill $RUNTIME_PID 2>/dev/null; exit 1" INT TERM

while [ $SAMPLE_COUNT -lt $TOTAL_SAMPLES ]; do
    # 检查进程是否仍在运行
    if ! kill -0 $RUNTIME_PID 2>/dev/null; then
        echo "警告：运行时程序已退出"
        echo "退出时间: $(date)" >> $LOG_FILE
        break
    fi

    # 读取当前内存
    CURRENT_MEM=$(ps -o rss= -p $RUNTIME_PID)

    if [ -z "$CURRENT_MEM" ]; then
        echo "警告：无法读取进程内存，进程可能已退出"
        break
    fi

    # 计算增长
    MEM_GROWTH=$((CURRENT_MEM - INITIAL_MEM))
    GROWTH_PERCENT=$(awk "BEGIN {printf \"%.2f\", ($MEM_GROWTH / $INITIAL_MEM) * 100}")

    # 记录到日志
    TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")
    echo "$TIMESTAMP,$CURRENT_MEM,$MEM_GROWTH,$GROWTH_PERCENT" >> $LOG_FILE

    # 显示进度
    ELAPSED_HOURS=$(awk "BEGIN {printf \"%.1f\", $SAMPLE_COUNT * $SAMPLE_INTERVAL_SEC / 3600}")
    echo "[$ELAPSED_HOURS/$TEST_DURATION_HOURS 小时] 内存: $CURRENT_MEM KB (+$MEM_GROWTH KB, +$GROWTH_PERCENT%)"

    # 检查是否有显著内存增长（超过 10%）
    if [ $MEM_GROWTH -gt $((INITIAL_MEM / 10)) ]; then
        echo "⚠️  警告：内存增长超过 10% ($GROWTH_PERCENT%)"
    fi

    SAMPLE_COUNT=$((SAMPLE_COUNT + 1))
    sleep $SAMPLE_INTERVAL_SEC
done

# 停止运行时程序
echo ""
echo "停止运行时程序..."
kill -TERM $RUNTIME_PID 2>/dev/null
sleep 2

# 如果进程仍在运行，强制杀死
if kill -0 $RUNTIME_PID 2>/dev/null; then
    kill -KILL $RUNTIME_PID 2>/dev/null
fi

# 读取最终内存
FINAL_MEM=$(tail -1 $LOG_FILE | cut -d',' -f2)
TOTAL_GROWTH=$((FINAL_MEM - INITIAL_MEM))
TOTAL_GROWTH_PERCENT=$(awk "BEGIN {printf \"%.2f\", ($TOTAL_GROWTH / $INITIAL_MEM) * 100}")

# 生成报告
echo ""
echo "========================================"
echo "测试完成"
echo "========================================"
echo "结束时间: $(date)" | tee -a $LOG_FILE
echo "总运行时间: $ELAPSED_HOURS 小时" | tee -a $LOG_FILE
echo "初始内存: $INITIAL_MEM KB" | tee -a $LOG_FILE
echo "最终内存: $FINAL_MEM KB" | tee -a $LOG_FILE
echo "内存增长: $TOTAL_GROWTH KB ($TOTAL_GROWTH_PERCENT%)" | tee -a $LOG_FILE
echo ""

# 分析结果
if [ $TOTAL_GROWTH -lt $((INITIAL_MEM / 20)) ]; then
    echo "✓ 测试通过：内存增长在可接受范围内 (<5%)" | tee -a $LOG_FILE
    echo "未检测到明显的内存泄漏" | tee -a $LOG_FILE
    exit 0
elif [ $TOTAL_GROWTH -lt $((INITIAL_MEM / 10)) ]; then
    echo "⚠ 测试警告：内存增长较大 (5-10%)" | tee -a $LOG_FILE
    echo "建议进一步调查" | tee -a $LOG_FILE
    exit 0
else
    echo "✗ 测试失败：内存增长超过 10%" | tee -a $LOG_FILE
    echo "可能存在内存泄漏，需要使用 Valgrind 详细分析" | tee -a $LOG_FILE
    exit 1
fi
