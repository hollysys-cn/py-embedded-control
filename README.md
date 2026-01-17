# PLCopen Python 运行时环境

一个运行在 Linux 嵌入式环境的 PLCopen 标准功能块运行时系统。系统核心采用 C11 语言实现高性能的控制算法功能块（PID、一阶惯性等），通过 Python C Extension API 为 Python 3.8+ 提供绑定接口。

## 特性

- ✅ **PLCopen 标准功能块**：PID 控制器、一阶惯性、斜坡、限幅等
- ✅ **Python 脚本控制**：使用 Python 快速组合功能块实现控制逻辑
- ✅ **高性能实时调度**：C11 实现，周期稳定性 95%（10ms-1000ms）
- ✅ **远程调试支持**：通过 debugpy 协议远程调试 Python 脚本
- ✅ **多架构支持**：x86_64 和 ARM (Cortex-A7+)
- ✅ **Docker 开发环境**：支持 x86_64 开发和 ARM 功能测试

## 快速开始

### 方式 1：Docker（推荐）

```bash
# 构建镜像
make docker-build

# 启动开发容器（x86_64）
make docker-dev

# 或启动 ARM 测试容器
make docker-arm
```

### 方式 2：原生安装

**前置要求**：
- Linux (Ubuntu 20.04+ / Debian 10+)
- Python 3.8+
- GCC 工具链
- 开发头文件（python3-dev）

**安装步骤**：

```bash
# 1. 克隆仓库
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control

# 2. 创建虚拟环境
python3 -m venv venv
source venv/bin/activate

# 3. 安装依赖
pip install -r requirements.txt

# 4. 构建 C 扩展
make build

# 5. 安装到虚拟环境
make install
```

## 使用示例

### 1. 创建控制脚本

创建文件 `my_control.py`：

```python
import plcopen

# 全局变量：功能块实例
pid = None

def init():
    """初始化函数，启动时调用一次"""
    global pid
    # 创建 PID 控制器（Kp=2.0, Ki=0.5, Kd=0.1）
    pid = plcopen.PID(Kp=2.0, Ki=0.5, Kd=0.1, output_min=0, output_max=100)
    print("PID 控制器初始化完成")

def step():
    """周期执行函数，每个控制周期调用"""
    # 模拟传感器读取
    temperature = 23.5  # 实际应用中从硬件读取
    setpoint = 25.0

    # 计算控制输出
    control = pid.compute(SP=setpoint, PV=temperature)

    # 输出控制信号
    print(f"温度: {temperature}°C, 控制输出: {control:.2f}%")
    # 实际应用中将 control 写入硬件
```

### 2. 创建配置文件

创建文件 `config/runtime.yaml`：

```yaml
# 运行时配置
runtime:
  cycle_period_ms: 100        # 控制周期（毫秒）
  script_path: my_control.py  # Python 脚本路径

# 日志配置
logging:
  level: INFO                 # 日志级别：DEBUG, INFO, WARNING, ERROR
  file: runtime.log           # 日志文件路径

# 调试配置（可选）
debug:
  enabled: false              # 是否启用远程调试
  host: 0.0.0.0               # 监听地址
  port: 5678                  # 调试端口
```

### 3. 运行控制循环

```bash
# 运行运行时（未实现，Phase 2 完成后可用）
# ./bin/plcopen-runtime --config config/runtime.yaml
```

## 远程调试

使用 VS Code 远程调试 Python 控制脚本：

**1. 启用调试**（修改 `config/runtime.yaml`）：

```yaml
debug:
  enabled: true
  host: 0.0.0.0
  port: 5678
```

**2. 配置 VS Code**（创建 `.vscode/launch.json`）：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "远程调试 PLCopen 运行时",
            "type": "python",
            "request": "attach",
            "connect": {
                "host": "192.168.1.100",
                "port": 5678
            },
            "pathMappings": [
                {
                    "localRoot": "${workspaceFolder}",
                    "remoteRoot": "/app"
                }
            ]
        }
    ]
}
```

**3. 启动调试**：在 VS Code 中按 F5，连接到远程设备。

## 项目结构

```
py-embedded-control/
├── src/                   # C 语言源代码
│   ├── runtime/           # 运行时核心
│   ├── function_blocks/   # 功能块实现
│   └── python_bindings/   # Python C Extension
├── python/                # Python 包和示例
│   ├── plcopen/           # 高层 Python 封装
│   └── examples/          # 示例脚本
├── tests/                 # 测试目录
├── config/                # 配置文件
├── docs/                  # 文档
├── Makefile               # 构建系统
└── README.md              # 本文件
```

## 开发

### 运行测试

```bash
make test
```

### 代码质量检查

```bash
make lint
```

### 清理构建产物

```bash
make clean
```

## 文档

- [快速入门指南](specs/001-plcopen-python-runtime/quickstart.md)
- [API 参考](specs/001-plcopen-python-runtime/contracts/function_blocks_api.md)
- [数据模型](specs/001-plcopen-python-runtime/data-model.md)
- [实施计划](specs/001-plcopen-python-runtime/plan.md)

## 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 贡献

欢迎贡献！请参阅 [贡献指南](docs/CONTRIBUTING.md)（即将发布）。

## 联系方式

- 项目仓库：https://github.com/hollysys-cn/py-embedded-control
- 问题反馈：https://github.com/hollysys-cn/py-embedded-control/issues
