#!/usr/bin/env bash
#
# test.sh - 运行项目测试和静态分析
#
# 用法: ./test.sh [OPTIONS]
# 选项:
#   --lint      仅运行静态分析
#   --unit      仅运行单元测试
#   --help      显示此帮助信息
#

set -e  # 遇到错误立即退出

# 颜色定义(仅在 TTY 时生效)
if [ -t 1 ]; then
    GREEN='\033[0;32m'
    RED='\033[0;31m'
    YELLOW='\033[0;33m'
    NC='\033[0m'  # No Color
else
    GREEN=''
    RED=''
    YELLOW=''
    NC=''
fi

# 输出函数
info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

# 帮助信息
show_help() {
    cat << EOF
用法: ./test.sh [OPTIONS]

运行项目测试和静态分析

选项:
    --lint      仅运行静态分析(flake8)
    --unit      仅运行单元测试(pytest)
    --help      显示此帮助信息

示例:
    ./test.sh               # 运行所有测试
    ./test.sh --lint        # 仅静态分析
    ./test.sh --unit        # 仅单元测试

EOF
}

# 主函数
main() {
    local run_lint=false
    local run_unit=false

    # 参数解析
    while [[ $# -gt 0 ]]; do
        case $1 in
            --lint)
                run_lint=true
                shift
                ;;
            --unit)
                run_unit=true
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                error "未知参数: $1"
                echo "使用 --help 查看帮助信息" >&2
                exit 2
                ;;
        esac
    done

    # 如果没有指定任何标志,运行所有测试
    if [ "$run_lint" = false ] && [ "$run_unit" = false ]; then
        run_lint=true
        run_unit=true
    fi

    # 确保容器正在运行
    if ! docker ps --format '{{.Names}}' | grep -q "plcopen-dev"; then
        error "开发容器未运行,请先执行 ./build.sh"
        exit 1
    fi

    local test_failed=false

    # 运行静态分析
    if [ "$run_lint" = true ]; then
        info "运行静态分析..."
        info "运行 flake8..."

        if docker exec plcopen-dev bash -c "cd /workspace && python3 -m flake8 python/ --max-line-length=100 --exclude=__pycache__"; then
            info "静态分析通过"
        else
            error "静态分析发现问题"
            test_failed=true
        fi
    fi

    # 运行单元测试
    if [ "$run_unit" = true ]; then
        info "运行单元测试..."

        if docker exec plcopen-dev bash -c "cd /workspace && python3 -m pytest tests/unit/ -v"; then
            info "单元测试通过"
        else
            error "单元测试失败"
            test_failed=true
        fi
    fi

    # 返回测试结果
    if [ "$test_failed" = true ]; then
        error "测试失败"
        exit 1
    else
        info "所有测试通过"
        exit 0
    fi
}

main "$@"
