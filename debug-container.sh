#!/bin/bash
# 容器调试快速启动脚本（Linux/macOS）

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
GRAY='\033[0;90m'
NC='\033[0m' # No Color

# 默认参数
PLATFORM="x86_64"
BUILD_IMAGE=false
WAIT_DEBUGGER=false
RUN_EXAMPLE=""

# 帮助信息
show_help() {
    cat << EOF
用法: $0 [选项]

容器调试快速启动脚本

选项:
    -p, --platform PLATFORM    目标平台: x86_64 或 arm64 (默认: x86_64)
    -b, --build                重新构建镜像
    -w, --wait                 等待调试器连接
    -e, --example EXAMPLE      运行示例: pid_temperature, basic_pid, ramp_limit
    -h, --help                 显示此帮助信息

示例:
    $0                                    # 启动 x86_64 容器
    $0 -p arm64 -b                        # 重新构建并启动 ARM64 容器
    $0 -e pid_temperature -w              # 运行示例并等待调试器
EOF
}

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--platform)
            PLATFORM="$2"
            shift 2
            ;;
        -b|--build)
            BUILD_IMAGE=true
            shift
            ;;
        -w|--wait)
            WAIT_DEBUGGER=true
            shift
            ;;
        -e|--example)
            RUN_EXAMPLE="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}错误: 未知选项 $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# 检查 Docker
echo -e "${CYAN}🔍 检查 Docker 环境...${NC}"
if ! docker info &> /dev/null; then
    echo -e "${RED}✗ Docker 未运行，请启动 Docker${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Docker 正在运行${NC}"

# 确定服务名称和端口
if [ "$PLATFORM" = "arm64" ]; then
    SERVICE_NAME="arm-test"
    CONTAINER_NAME="plcopen-arm-test"
    DEBUG_PORT="5679"
    GDB_PORT="9001"
else
    SERVICE_NAME="dev"
    CONTAINER_NAME="plcopen-dev"
    DEBUG_PORT="5678"
    GDB_PORT="9000"
fi

echo -e "\n${YELLOW}📦 目标平台: $PLATFORM${NC}"
echo -e "${YELLOW}📦 服务名称: $SERVICE_NAME${NC}"
echo -e "${YELLOW}📦 容器名称: $CONTAINER_NAME${NC}"
echo -e "${YELLOW}📦 调试端口: $DEBUG_PORT${NC}"

# 构建镜像
if [ "$BUILD_IMAGE" = true ]; then
    echo -e "\n${CYAN}🔨 构建 Docker 镜像...${NC}"
    docker-compose build $SERVICE_NAME
    echo -e "${GREEN}✓ 镜像构建成功${NC}"
fi

# 清理现有容器
echo -e "\n${CYAN}🧹 清理现有容器...${NC}"
docker-compose down $SERVICE_NAME 2>/dev/null || true

# 启动容器
echo -e "\n${CYAN}🚀 启动容器...${NC}"
docker-compose up -d $SERVICE_NAME

# 等待容器就绪
echo -e "${CYAN}⏳ 等待容器就绪...${NC}"
sleep 2

# 检查容器状态
if [ "$(docker inspect -f '{{.State.Running}}' $CONTAINER_NAME 2>/dev/null)" != "true" ]; then
    echo -e "${RED}✗ 容器未运行${NC}"
    docker logs $CONTAINER_NAME
    exit 1
fi
echo -e "${GREEN}✓ 容器正在运行${NC}"

# 显示容器信息
echo -e "\n${CYAN}📊 容器信息:${NC}"
docker exec $CONTAINER_NAME python3 --version
docker exec $CONTAINER_NAME gcc --version | head -n 1

# 重新构建项目
echo -e "\n${CYAN}🔧 在容器内构建项目...${NC}"
docker exec $CONTAINER_NAME bash -c "cd /workspace && python3 setup.py build_ext --inplace" || \
    echo -e "${YELLOW}⚠ Python 扩展构建失败，但继续运行${NC}"

docker exec $CONTAINER_NAME bash -c "cd /workspace && make runtime" || \
    echo -e "${YELLOW}⚠ 运行时构建失败，但继续运行${NC}"

# 运行示例
if [ -n "$RUN_EXAMPLE" ]; then
    echo -e "\n${CYAN}🎯 准备运行示例: $RUN_EXAMPLE${NC}"

    EXAMPLE_PATH="/workspace/python/examples/${RUN_EXAMPLE}.py"
    DEBUG_ARGS=""
    [ "$WAIT_DEBUGGER" = true ] && DEBUG_ARGS="--wait-for-client"

    echo -e "${CYAN}🐛 启动 debugpy 服务器...${NC}"
    DEBUG_COMMAND="python3 -m debugpy --listen 0.0.0.0:5678 $DEBUG_ARGS $EXAMPLE_PATH"

    echo -e "\n${YELLOW}💡 在容器中执行命令:${NC}"
    echo -e "${GRAY}   docker exec -it $CONTAINER_NAME bash -c \"$DEBUG_COMMAND\"${NC}"

    if [ "$WAIT_DEBUGGER" = true ]; then
        echo -e "\n${MAGENTA}⏸ 等待调试器连接到 localhost:$DEBUG_PORT${NC}"
        echo -e "${GRAY}   1. 在 VS Code 中按 F5 选择 'Python: 附加到 Docker 容器 ($PLATFORM)'${NC}"
        echo -e "${GRAY}   2. 调试器连接后程序将继续执行${NC}"
    fi

    docker exec -it $CONTAINER_NAME bash -c "$DEBUG_COMMAND"
else
    echo -e "\n${GREEN}✅ 容器已启动并保持运行${NC}"
    echo -e "\n${CYAN}📝 下一步操作:${NC}"
    echo -e "${GRAY}   1. 使用 VS Code 'Remote - Containers' 扩展连接到容器${NC}"
    echo -e "${GRAY}   2. 或者手动进入容器: docker exec -it $CONTAINER_NAME bash${NC}"
    echo -e "${GRAY}   3. 启动调试:${NC}"
    echo -e "${GRAY}      - Python 调试: python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py${NC}"
    echo -e "${GRAY}      - C 调试: gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml${NC}"
    echo -e "${GRAY}   4. 在 VS Code 中按 F5 选择对应的调试配置${NC}"
    echo -e "\n${CYAN}🔗 调试端口: localhost:$DEBUG_PORT (Python), localhost:$GDB_PORT (gdbserver)${NC}"
fi

echo -e "\n${GREEN}🎉 容器调试环境已就绪！${NC}"
