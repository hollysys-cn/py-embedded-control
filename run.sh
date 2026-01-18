#!/usr/bin/env bash
#
# run.sh - 运行 PLCopen Python Runtime 应用
#
# 用法: ./run.sh [CONFIG_FILE] [OPTIONS]
# 参数:
#   CONFIG_FILE  配置文件路径(默认: config/pid_temperature.yaml)
# 选项:
#   --shell      进入容器 Shell 而非运行应用
#   --help       显示此帮助信息
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
用法: ./run.sh [CONFIG_FILE] [OPTIONS]

运行 PLCopen Python Runtime 应用

参数:
    CONFIG_FILE  配置文件路径(默认: config/pid_temperature.yaml)

选项:
    --shell      进入容器 Shell 而非运行应用
    --help       显示此帮助信息

示例:
    ./run.sh                                    # 使用默认配置运行
    ./run.sh config/custom.yaml                 # 使用自定义配置
    ./run.sh --shell                            # 进入容器 Shell

EOF
}

# 主函数
main() {
    local config_file="config/pid_temperature.yaml"
    local shell_mode=false

    # 参数解析
    while [[ $# -gt 0 ]]; do
        case $1 in
            --shell)
                shell_mode=true
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            -*)
                error "未知参数: $1"
                echo "使用 --help 查看帮助信息" >&2
                exit 2
                ;;
            *)
                config_file="$1"
                shift
                ;;
        esac
    done

    # Shell 模式
    if [ "$shell_mode" = true ]; then
        info "进入容器 Shell..."
        info "输入 'exit' 退出 Shell"
        docker exec -it plcopen-dev bash
        exit 0
    fi

    # 验证配置文件存在
    if [ ! -f "$config_file" ]; then
        error "配置文件不存在: $config_file"
        exit 1
    fi

    # 确保容器正在运行
    if ! docker ps --format '{{.Names}}' | grep -q "plcopen-dev"; then
        info "启动开发容器..."
        docker-compose up -d dev
        sleep 2  # 等待容器启动
    fi

    # 运行应用
    info "使用配置文件启动应用: $config_file"

    # 使用 MSYS_NO_PATHCONV 避免 Git Bash 在 Windows 上转换路径
    MSYS_NO_PATHCONV=1 docker exec -it plcopen-dev bash -c \
        "cd /workspace && python3 python/examples/pid_temperature.py --config $config_file"

    exit 0
}

main "$@"
