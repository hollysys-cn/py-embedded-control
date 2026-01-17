# PLCopen Python 运行时环境 Dockerfile
# 支持多架构：x86_64 和 ARM64

FROM debian:stable-slim

# 设置阿里云 Debian 镜像和 PyPI 镜像
RUN sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list.d/debian.sources && \
    mkdir -p /root/.pip && \
    echo "[global]" > /root/.pip/pip.conf && \
    echo "index-url = https://mirrors.aliyun.com/pypi/simple/" >> /root/.pip/pip.conf && \
    echo "trusted-host = mirrors.aliyun.com" >> /root/.pip/pip.conf

# 安装构建工具和依赖
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        gcc \
        g++ \
        make \
        python3 \
        python3-dev \
        python3-pip \
        python3-venv \
        cppcheck \
        git \
        gdb \
        strace \
        valgrind \
        vim \
        procps \
        net-tools \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 复制项目文件
COPY requirements.txt ./
RUN python3 -m pip install --no-cache-dir --break-system-packages setuptools wheel && \
    python3 -m pip install --no-cache-dir --break-system-packages -r requirements.txt

COPY . .

# 构建 C 扩展和运行时可执行文件
RUN python3 setup.py build_ext --inplace && \
    mkdir -p bin && \
    make runtime

# 设置 Python 路径和调试环境变量
ENV PYTHONPATH=/app/python:/app/src/python_bindings \
    PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1

# 默认命令：运行 PID 温度控制示例
CMD ["bin/plcopen_runtime", "--config", "config/pid_temperature.yaml"]

# 健康检查（可选，Phase 2 实现后启用）
# HEALTHCHECK --interval=30s --timeout=3s --start-period=10s --retries=3 \
#     CMD pgrep -f plcopen-runtime || exit 1
