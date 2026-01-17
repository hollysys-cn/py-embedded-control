# PLCopen Python 运行时环境 - Makefile
# 用于构建 C 扩展和运行测试

# 编译器配置
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -fPIC -Isrc
PYTHON = python3
PYTHON_CONFIG = python3-config

# 目录定义
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Python 扩展配置
PYTHON_INCLUDES = $(shell $(PYTHON_CONFIG) --includes)
PYTHON_LIBS = $(shell $(PYTHON_CONFIG) --ldflags)

.PHONY: all build clean install test docker-build docker-dev docker-arm lint help

# 默认目标
all: build

# 构建 Python C 扩展
build:
	@echo "构建 Python C 扩展..."
	@mkdir -p $(BUILD_DIR)
	cd $(SRC_DIR)/python_bindings && $(PYTHON) setup.py build_ext --inplace
	@echo "构建完成"

# 清理构建产物
clean:
	@echo "清理构建产物..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -rf $(SRC_DIR)/python_bindings/build
	find . -type f -name "*.o" -delete
	find . -type f -name "*.so" -delete
	find . -type f -name "*.pyc" -delete
	find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find . -type d -name "*.egg-info" -exec rm -rf {} + 2>/dev/null || true
	@echo "清理完成"

# 安装到虚拟环境
install: build
	@echo "安装 Python 包..."
	$(PYTHON) -m pip install -e .
	@echo "安装完成"

# 运行测试
test:
	@echo "运行单元测试..."
	$(PYTHON) -m pytest $(TEST_DIR) -v
	@echo "测试完成"

# 构建 Docker 镜像（x86_64）
docker-build:
	@echo "构建 Docker 镜像（x86_64）..."
	docker build -t plcopen-runtime:latest .
	@echo "Docker 镜像构建完成"

# 启动开发容器（x86_64）
docker-dev:
	@echo "启动开发容器..."
	docker-compose up -d dev
	@echo "开发容器已启动，调试端口: 5678"

# 启动 ARM 测试容器
docker-arm:
	@echo "启动 ARM 测试容器..."
	docker-compose up -d arm-test
	@echo "ARM 测试容器已启动，调试端口: 5679"

# 代码静态分析
lint:
	@echo "运行静态分析..."
	@echo "检查 C 代码..."
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR) || true
	@echo "检查 Python 代码..."
	$(PYTHON) -m flake8 python/ --max-line-length=100 || true
	$(PYTHON) -m pylint python/ || true
	@echo "静态分析完成"

# 显示帮助信息
help:
	@echo "PLCopen Python 运行时环境 - Makefile"
	@echo ""
	@echo "可用目标："
	@echo "  make build        - 构建 Python C 扩展"
	@echo "  make clean        - 清理构建产物"
	@echo "  make install      - 安装到虚拟环境"
	@echo "  make test         - 运行单元测试"
	@echo "  make docker-build - 构建 Docker 镜像（x86_64）"
	@echo "  make docker-dev   - 启动开发容器（x86_64）"
	@echo "  make docker-arm   - 启动 ARM 测试容器"
	@echo "  make lint         - 运行代码静态分析"
	@echo "  make help         - 显示此帮助信息"
