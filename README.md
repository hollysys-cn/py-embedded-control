# PLCopen Python 运行时环境

一个运行在 Linux 嵌入式环境的 PLCopen 标准功能块运行时系统。系统核心采用 C11 语言实现高性能的控制算法功能块（PID、一阶惯性等），通过 Python C Extension API 为 Python 3.8+ 提供绑定接口。

## 🎯 项目状态

**当前版本**: 0.2.0 (Phase 6 完成)
**构建状态**: ✅ 通过（2026-01-17）
**测试状态**: ✅ 所有功能块测试通过
**实施进度**: Phase 6 完成（文档完善、代码质量、许可证合规）

### 完成阶段

- ✅ **Phase 1**: Setup（项目结构）
- ✅ **Phase 2**: Foundational（核心运行时）
- ✅ **Phase 3**: User Story 1（基础功能块）
- ✅ **Phase 4**: User Story 2（完整控制循环）
- ✅ **Phase 6**: Polish（文档、质量、合规）

### 待实施

- 🚧 **Phase 5**: User Story 3（远程调试）

## 特性

- ✅ **PLCopen 标准功能块**：PID 控制器、一阶惯性、斜率限制、限幅
- ✅ **Python 脚本控制**：使用 Python 快速组合功能块实现控制逻辑
- ✅ **高性能 C11 实现**：符合 IEC 61131-3 标准，优化的控制算法
- ✅ **Python C Extension API**：零开销的 Python 绑定
- ✅ **实时调度器**：纳秒级精度的周期调度（clock_nanosleep）
- ✅ **运行时环境**：完整的主程序和控制循环
- ✅ **Docker 开发环境**：开箱即用的构建和测试环境
- ✅ **温度控制示例**：带物理模型的 PID 温度控制演示
- 🚧 **远程调试支持**：debugpy 协议（Phase 5 计划中）
- 🚧 **多架构支持**：x86_64 已验证，ARM 测试中

## 快速开始

### 方式 1：Docker（推荐，支持 Windows/Mac/Linux）

#### Windows 环境

```powershell
# 1. 确保 Docker Desktop 正在运行

# 2. 构建项目
.\build.ps1 -All

# 3. 运行 PID 温度控制示例
.\run.ps1

# 4. 运行调试模式（支持 VS Code 远程调试）
.\run.ps1 -Debug

# 5. 进入开发环境 Shell
.\run.ps1 -Shell
```

#### Linux/Mac 环境

```bash
# 构建镜像
docker-compose build dev

# 构建运行时
docker-compose run --rm dev make runtime

# 构建 Python 扩展
docker-compose run --rm dev python3 setup.py build_ext --inplace

# 运行 PID 示例
docker-compose run --rm dev ./bin/plcopen_runtime --config config/pid_temperature.yaml

# 进入开发环境
docker-compose run --rm dev bash
```

### 方式 2：原生安装（仅 Linux）

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

# 4. 构建运行时和 Python 扩展
make runtime
make build

# 5. 运行示例
./bin/plcopen_runtime --config config/pid_temperature.yaml
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
# 方式 1：使用 Docker（推荐）
docker build -t plcopen-runtime .
docker run --rm plcopen-runtime

# 方式 2：直接运行示例
python3 python/examples/pid_temperature.py

# 方式 3：使用运行时可执行文件
bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 4. 查看示例输出

PID 温度控制示例输出：

```
PID 温度控制初始化完成
  目标温度: 25.0°C
  初始温度: 20.0°C
  PID 参数: Kp=3.0, Ki=0.2, Kd=0.5
------------------------------------------------------------
周期   10 | 温度: 21.07°C | 误差:  3.930°C | 控制输出:  12.3%
周期   20 | 温度: 21.52°C | 误差:  3.476°C | 控制输出:  10.4%
周期   30 | 温度: 21.72°C | 误差:  3.280°C | 控制输出:   9.6%
```

详细说明见 [PID 温度控制示例文档](docs/examples/pid_temperature.md)。

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

完整文档请参考：

- **[用户手册](docs/user_guide.md)** - 安装、配置、使用指南
- **[API 参考](docs/api_reference.md)** - 完整 API 文档
- **[常见问题](docs/faq.md)** - 常见问题和解决方案
- **[贡献指南](docs/contributing.md)** - 如何参与开发
- **[代码质量](docs/code_quality.md)** - 静态分析和质量检查
- **[许可证](LICENSE)** - MIT License
- **[依赖许可证](docs/licenses.md)** - 第三方依赖许可信息
- [快速入门指南](specs/001-plcopen-python-runtime/quickstart.md)
- [数据模型](specs/001-plcopen-python-runtime/data-model.md)
- [实施计划](specs/001-plcopen-python-runtime/plan.md)

## 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

第三方依赖许可证信息请参见 [docs/licenses.md](docs/licenses.md)。

## 贡献

欢迎贡献！请参阅 [贡献指南](docs/contributing.md)。

## 联系方式

- 项目仓库：https://github.com/hollysys-cn/py-embedded-control
- 问题反馈：https://github.com/hollysys-cn/py-embedded-control/issues
