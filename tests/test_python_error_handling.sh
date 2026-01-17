#!/bin/bash
# Python 脚本错误处理验证测试
# 验证运行时正确处理各种 Python 脚本错误

RUNTIME_BIN="./bin/plcopen_runtime"
TEST_DIR="tests/error_cases"
LOG_DIR="logs/error_tests"

echo "========================================"
echo "Python 脚本错误处理验证"
echo "========================================"
echo ""

# 创建测试目录
mkdir -p $TEST_DIR
mkdir -p $LOG_DIR

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 颜色定义（如果支持）
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 运行单个测试
run_test() {
    local test_name=$1
    local expected_behavior=$2
    local config_file=$3

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo ""
    echo "测试 $TOTAL_TESTS: $test_name"
    echo "----------------------------------------"

    # 运行程序并捕获输出
    timeout 3s $RUNTIME_BIN --config $config_file > $LOG_DIR/${test_name}.log 2>&1
    EXIT_CODE=$?

    # 检查期望的行为
    if [ "$expected_behavior" = "fail_with_error" ]; then
        # 期望失败并输出错误信息
        if [ $EXIT_CODE -ne 0 ]; then
            if grep -q "ERROR" $LOG_DIR/${test_name}.log || \
               grep -q "错误" $LOG_DIR/${test_name}.log || \
               grep -q "异常" $LOG_DIR/${test_name}.log; then
                echo -e "${GREEN}✓ 通过${NC}：程序正确检测到错误并失败"
                PASSED_TESTS=$((PASSED_TESTS + 1))
                return 0
            fi
        fi
        echo -e "${RED}✗ 失败${NC}：程序未能正确处理错误"
        echo "退出代码: $EXIT_CODE"
        echo "输出:"
        cat $LOG_DIR/${test_name}.log
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    elif [ "$expected_behavior" = "warn_and_continue" ]; then
        # 期望警告但继续运行
        if grep -q "WARN" $LOG_DIR/${test_name}.log || \
           grep -q "警告" $LOG_DIR/${test_name}.log; then
            echo -e "${GREEN}✓ 通过${NC}：程序输出警告并继续运行"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            return 0
        fi
        echo -e "${YELLOW}⚠ 部分通过${NC}：程序运行但未输出警告"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    fi

    return 1
}

# 测试 1：缺少 init() 函数
echo ""
echo "准备测试用例..."

cat > $TEST_DIR/missing_init.py << 'EOF'
"""缺少 init() 函数的脚本"""

def step():
    """只有 step 函数，没有 init"""
    print("Step function")
EOF

cat > $TEST_DIR/missing_init_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/missing_init.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "missing_init" "fail_with_error" "$TEST_DIR/missing_init_config.yaml"

# 测试 2：缺少 step() 函数
cat > $TEST_DIR/missing_step.py << 'EOF'
"""缺少 step() 函数的脚本"""

def init():
    """只有 init 函数，没有 step"""
    print("Init function")
EOF

cat > $TEST_DIR/missing_step_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/missing_step.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "missing_step" "fail_with_error" "$TEST_DIR/missing_step_config.yaml"

# 测试 3：init() 函数抛出异常
cat > $TEST_DIR/init_exception.py << 'EOF'
"""init() 函数抛出异常"""

def init():
    """初始化时抛出异常"""
    raise ValueError("测试异常：初始化失败")

def step():
    """正常的 step 函数"""
    print("Step")
EOF

cat > $TEST_DIR/init_exception_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/init_exception.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "init_exception" "fail_with_error" "$TEST_DIR/init_exception_config.yaml"

# 测试 4：step() 函数抛出异常
cat > $TEST_DIR/step_exception.py << 'EOF'
"""step() 函数抛出异常"""

def init():
    """正常初始化"""
    print("Init OK")

def step():
    """每次调用都抛出异常"""
    raise RuntimeError("测试异常：step 执行失败")
EOF

cat > $TEST_DIR/step_exception_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/step_exception.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "step_exception" "fail_with_error" "$TEST_DIR/step_exception_config.yaml"

# 测试 5：语法错误的 Python 脚本
cat > $TEST_DIR/syntax_error.py << 'EOF'
"""包含语法错误的脚本"""

def init():
    """缺少冒号导致语法错误"""
    print("Init")
    if True  # <-- 语法错误：缺少冒号
        pass

def step():
    print("Step")
EOF

cat > $TEST_DIR/syntax_error_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/syntax_error.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "syntax_error" "fail_with_error" "$TEST_DIR/syntax_error_config.yaml"

# 测试 6：导入不存在的模块
cat > $TEST_DIR/import_error.py << 'EOF'
"""导入不存在的模块"""
import nonexistent_module_12345  # <-- 不存在的模块

def init():
    print("Init")

def step():
    print("Step")
EOF

cat > $TEST_DIR/import_error_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/import_error.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "import_error" "fail_with_error" "$TEST_DIR/import_error_config.yaml"

# 测试 7：函数不可调用（变量而非函数）
cat > $TEST_DIR/not_callable.py << 'EOF'
"""init/step 不是函数"""

init = "This is not a function"  # <-- 变量而非函数

def step():
    print("Step")
EOF

cat > $TEST_DIR/not_callable_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/not_callable.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "not_callable" "fail_with_error" "$TEST_DIR/not_callable_config.yaml"

# 测试 8：运行时类型错误
cat > $TEST_DIR/type_error.py << 'EOF'
"""运行时类型错误"""

def init():
    """初始化成功"""
    print("Init OK")

def step():
    """尝试进行非法操作"""
    x = "string"
    y = x + 123  # <-- TypeError: can only concatenate str to str
    print(y)
EOF

cat > $TEST_DIR/type_error_config.yaml << 'EOF'
cycle_period_ms: 100
python_script: "tests/error_cases/type_error.py"
function_blocks:
  - type: pid
    name: pid_test
    settings:
      kp: 1.0
      ki: 0.1
      kd: 0.05
EOF

run_test "type_error" "fail_with_error" "$TEST_DIR/type_error_config.yaml"

# 生成测试报告
echo ""
echo "========================================"
echo "测试报告"
echo "========================================"
echo "总测试数: $TOTAL_TESTS"
echo -e "通过: ${GREEN}$PASSED_TESTS${NC}"
echo -e "失败: ${RED}$FAILED_TESTS${NC}"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✓ 所有测试通过${NC}"
    echo "Python 脚本错误处理验证成功"
    exit 0
else
    echo -e "${RED}✗ 部分测试失败${NC}"
    echo "请检查日志文件: $LOG_DIR/"
    exit 1
fi
