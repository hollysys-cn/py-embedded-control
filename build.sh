#!/usr/bin/env bash
#
# build.sh - 构建 PLCopen Python Runtime 项目
#
# 用法: ./build.sh [OPTIONS]
# 选项:
#   --clean     清理构建产物后再构建
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
用法: ./build.sh [OPTIONS]

构建 PLCopen Python Runtime 项目

选项:
    --clean     清理构建产物后再构建
    --help      显示此帮助信息

示例:
    ./build.sh              # 正常构建
    ./build.sh --clean      # 清理后构建

EOF
}

# 主函数
main() {
    local clean=false

    # 参数解析
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                clean=true
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

    # 清理构建产物
    if [ "$clean" = true ]; then
        info "清理构建产物..."

        # 删除本地构建目录
        if [ -d "build" ]; then
            rm -rf build
            info "已删除 build/ 目录"
        fi

        if [ -d "bin" ]; then
            rm -rf bin
            info "已删除 bin/ 目录"
        fi

        # 在容器中执行 make clean (如果容器正在运行)
        if docker ps --format '{{.Names}}' | grep -q "plcopen-dev"; then
            info "在容器中执行 make clean..."
            docker exec plcopen-dev bash -c "cd /workspace && make clean" 2>&1 || true
        fi

        info "清理完成"
    fi

    # 构建 Docker 镜像
    info "构建 Docker 镜像..."
    docker-compose build dev

    if [ $? -eq 0 ]; then
        info "Docker 镜像构建成功"
    else
        error "Docker 镜像构建失败"
        exit 1
    fi

    # 启动容器(如果未运行)
    if ! docker ps --format '{{.Names}}' | grep -q "plcopen-dev"; then
        info "启动开发容器..."
        docker-compose up -d dev
    fi

    # 在容器中构建项目
    info "在容器中构建项目..."
    docker exec plcopen-dev bash -c "cd /workspace && python3 setup.py build_ext --inplace && make runtime"

    if [ $? -eq 0 ]; then
        info "构建完成"
    else
        error "项目构建失败"
        exit 1
    fi

    exit 0
}

main "$@"
