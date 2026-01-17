#!/usr/bin/env bash
# PLCopen Python 运行时 - 自动启动远程调试
# 用途：一键启动 Docker 容器、调试服务器并打开 VS Code 调试会话

set -euo pipefail

# 参数解析
REBUILD=false
NO_WAIT=false
EXAMPLE="pid_temperature"

while [[ $# -gt 0 ]]; do
    case $1 in
        --rebuild)
            REBUILD=true
            shift
            ;;
        --no-wait)
            NO_WAIT=true
            shift
            ;;
        --example)
            EXAMPLE="$2"
            shift 2
            ;;
        *)
            echo "未知参数: $1"
            echo "用法: $0 [--rebuild] [--no-wait] [--example <name>]"
            exit 1
            ;;
    esac
done

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
GRAY='\033[0;90m'
NC='\033[0m' # No Color

echo -e "${CYAN}=== PLCopen Python 运行时 - 远程调试自动启动 ===${NC}"
echo ""

# 步骤 1：检查 Docker 是否运行
echo -e "${YELLOW}[1/5] 检查 Docker 服务...${NC}"
if ! docker ps &>/dev/null; then
    echo -e "${RED}  ✗ Docker 服务未运行，请先启动 Docker${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ Docker 服务正常运行${NC}"

# 步骤 2：启动或重启容器
echo -e "${YELLOW}[2/5] 准备 Docker 容器...${NC}"

CONTAINER_RUNNING=$(docker ps --filter "name=plcopen-dev" --format "{{.Names}}" 2>/dev/null || true)
if [[ "$CONTAINER_RUNNING" == "plcopen-dev" ]]; then
    if [[ "$REBUILD" == "true" ]]; then
        echo -e "${GRAY}  停止现有容器...${NC}"
        docker-compose down dev &>/dev/null
        echo -e "${GRAY}  重新构建并启动容器...${NC}"
        docker-compose up -d --build dev
    else
        echo -e "${GREEN}  ✓ 容器已运行，跳过启动${NC}"
    fi
else
    echo -e "${GRAY}  启动开发容器...${NC}"
    if [[ "$REBUILD" == "true" ]]; then
        docker-compose up -d --build dev
    else
        docker-compose up -d dev
    fi
fi

# 等待容器完全启动
sleep 2

# 验证容器状态
CONTAINER_STATUS=$(docker inspect plcopen-dev --format "{{.State.Running}}" 2>/dev/null || true)
if [[ "$CONTAINER_STATUS" != "true" ]]; then
    echo -e "${RED}  ✗ 容器启动失败${NC}"
    docker logs plcopen-dev --tail 50
    exit 1
fi

echo -e "${GREEN}  ✓ 容器运行正常${NC}"
CONTAINER_ID=$(docker ps --filter "name=plcopen-dev" --format "{{.ID}}")
echo -e "${GRAY}    容器 ID: $CONTAINER_ID${NC}"
echo -e "${GRAY}    端口映射: 5678:5678 (debugpy), 9000:9000 (gdbserver)${NC}"

# 步骤 3：构建项目（如果需要）
echo -e "${YELLOW}[3/5] 构建项目...${NC}"
if docker exec plcopen-dev bash -c "cd /workspace && python3 setup.py build_ext --inplace" &>/dev/null; then
    echo -e "${GREEN}  ✓ 项目构建成功${NC}"
else
    echo -e "${YELLOW}  ⚠ 构建警告（可能已构建）${NC}"
fi

# 步骤 4：启动调试服务器
echo -e "${YELLOW}[4/5] 启动 debugpy 服务器...${NC}"

# 停止可能存在的旧调试进程
docker exec plcopen-dev pkill -f "debugpy" &>/dev/null || true

# 确定示例脚本路径
SCRIPT_PATH="python/examples/$EXAMPLE.py"
echo -e "${GRAY}  示例脚本: $SCRIPT_PATH${NC}"

# 构建启动命令
DEBUG_CMD="cd /workspace && python3 -Xfrozen_modules=off -m debugpy --listen 0.0.0.0:5678"
if [[ "$NO_WAIT" != "true" ]]; then
    DEBUG_CMD="$DEBUG_CMD --wait-for-client"
fi
DEBUG_CMD="$DEBUG_CMD $SCRIPT_PATH"

echo -e "${GRAY}  启动命令: python3 -m debugpy --listen 0.0.0.0:5678 $([ "$NO_WAIT" != "true" ] && echo "--wait-for-client") $SCRIPT_PATH${NC}"

# 在后台启动调试服务器
docker exec -d plcopen-dev bash -c "$DEBUG_CMD" &>/dev/null

# 等待调试服务器启动
sleep 2

# 验证调试服务器
DEBUG_PID=$(docker exec plcopen-dev pgrep -f "debugpy" 2>/dev/null || true)
if [[ -z "$DEBUG_PID" ]]; then
    echo -e "${RED}  ✗ debugpy 服务器启动失败${NC}"
    docker logs plcopen-dev --tail 20
    exit 1
fi

echo -e "${GREEN}  ✓ debugpy 服务器运行中 (PID: $DEBUG_PID)${NC}"
echo -e "${GRAY}    监听地址: 0.0.0.0:5678${NC}"
if [[ "$NO_WAIT" != "true" ]]; then
    echo -e "${GRAY}    等待客户端连接...${NC}"
fi

# 步骤 5：提示用户附加调试器
echo -e "${YELLOW}[5/5] 准备附加调试器...${NC}"
echo ""
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}  调试环境已就绪！${NC}"
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}下一步：在 VS Code 中附加调试器${NC}"
echo ""
echo -e "  1. 按 Ctrl+Shift+D 打开'运行和调试'视图"
echo -e "  2. 选择: 'Python: 附加到 Docker 容器 (x86_64)'"
echo -e "  3. 按 F5 开始调试"
echo ""
echo -e "${GRAY}或者运行以下命令手动附加：${NC}"
echo -e "${CYAN}  code --command workbench.action.debug.start${NC}"
echo ""
echo -e "${YELLOW}调试技巧：${NC}"
echo -e "  • 在 python/examples/$EXAMPLE.py 中设置断点"
echo -e "  • 使用 F10 单步执行，F11 步入函数"
echo -e "  • 在'变量'面板查看实时数据"
echo -e "  • 在'调试控制台'评估表达式"
echo ""
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# 提供快速命令参考
echo -e "${YELLOW}常用命令：${NC}"
echo -e "${GRAY}  查看容器日志:  docker logs -f plcopen-dev${NC}"
echo -e "${GRAY}  进入容器 Shell: docker exec -it plcopen-dev bash${NC}"
echo -e "${GRAY}  停止调试:      docker exec plcopen-dev pkill -f debugpy${NC}"
echo -e "${GRAY}  停止容器:      docker-compose down${NC}"
echo ""

# 可选：自动打开 VS Code
echo -ne "是否自动打开 VS Code 并启动调试？ (y/N): "
read -r OPEN_VSCODE

if [[ "$OPEN_VSCODE" == "y" || "$OPEN_VSCODE" == "Y" ]]; then
    echo -e "${GRAY}  正在启动 VS Code 调试会话...${NC}"
    sleep 1
    code --command workbench.action.debug.start || true
    echo -e "${GREEN}  ✓ VS Code 调试会话已启动${NC}"
else
    echo -e "${GRAY}  跳过自动启动，请手动在 VS Code 中附加调试器${NC}"
fi

echo ""
echo -e "${CYAN}脚本执行完成！${NC}"
