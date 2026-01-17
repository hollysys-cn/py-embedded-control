# PLCopen Python 运行时环境 - 用户手册

## 目录

1. [简介](#简介)
2. [系统要求](#系统要求)
3. [安装](#安装)
4. [快速开始](#快速开始)
5. [配置](#配置)
6. [编写控制脚本](#编写控制脚本)
7. [功能块参考](#功能块参考)
8. [运行和调试](#运行和调试)
9. [常见问题](#常见问题)

---

## 简介

PLCopen Python 运行时环境是一个运行在 Linux 嵌入式设备上的实时控制系统。它提供：

- **高性能功能块**：用 C11 实现的 PLCopen 标准功能块（PID、一阶惯性、斜率限制、限幅）
- **Python 脚本控制**：使用 Python 快速组合功能块实现控制逻辑
- **实时调度**：纳秒级精度的周期调度，确保控制稳定性
- **Docker 支持**：开箱即用的开发和测试环境

## 系统要求

### 硬件要求

- **CPU**: ARM Cortex-A 或 x86_64
- **内存**: 至少 256MB RAM
- **存储**: 至少 100MB 可用空间

### 软件要求

- **操作系统**: Linux 内核 3.10+（支持 CLOCK_MONOTONIC）
- **Python**: Python 3.8 或更高版本
- **编译工具**（仅开发环境）:
  - GCC 7.0+ 或 Clang 6.0+
  - Python 开发头文件（python3-dev）
  - Make

## 安装

### 方式 1: Docker（推荐）

```bash
# 克隆仓库
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control

# 构建 Docker 镜像
docker build -t plcopen-runtime:latest .

# 运行示例
docker run --rm plcopen-runtime:latest
```

### 方式 2: 本地安装

```bash
# 1. 安装依赖
sudo apt-get update
sudo apt-get install -y python3 python3-dev python3-pip gcc make

# 2. 克隆仓库
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control

# 3. 构建 C 扩展
make build

# 4. 安装 Python 包
make install

# 5. 构建运行时可执行文件
make runtime
```

### 验证安装

```bash
# 测试运行时可执行文件
bin/plcopen_runtime --help

# 运行 Python 示例
python3 python/examples/pid_temperature.py
```

## 快速开始

### 1. 运行温度控制示例

```bash
# 使用 Docker
docker run --rm plcopen-runtime:latest

# 或本地运行
python3 python/examples/pid_temperature.py
```

### 2. 使用运行时环境

```bash
# 使用默认配置
bin/plcopen_runtime --config config/pid_temperature.yaml
```

## 配置

### 配置文件格式

配置文件使用 YAML 格式：

```yaml
# 日志配置
logging:
  level: INFO              # 日志级别：DEBUG, INFO, WARNING, ERROR
  file: logs/runtime.log   # 日志文件路径
  max_size: 1048576        # 日志文件最大大小（字节）

# 调度器配置
scheduler:
  cycle_time_ms: 100       # 控制周期（毫秒）
  timeout_ms: 50           # 超时检测阈值（毫秒）
  max_jitter_ms: 5         # 最大允许抖动（毫秒）

# Python 脚本配置
script:
  path: python/examples/pid_temperature.py  # 脚本路径
```

### 配置参数说明

#### logging 部分

- **level**: 日志级别
  - `DEBUG`: 详细调试信息
  - `INFO`: 一般信息（推荐）
  - `WARNING`: 警告信息
  - `ERROR`: 仅错误信息

- **file**: 日志文件路径（相对或绝对路径）

- **max_size**: 日志文件大小限制（字节），超过后自动轮转

#### scheduler 部分

- **cycle_time_ms**: 控制周期（毫秒）
  - 推荐范围：10-1000ms
  - 典型值：100ms（10Hz）

- **timeout_ms**: 超时检测阈值
  - 设为周期时间的 50-80%

- **max_jitter_ms**: 最大允许抖动
  - 设为周期时间的 5-10%

#### script 部分

- **path**: Python 脚本路径
  - 相对路径：相对于运行时工作目录
  - 绝对路径：完整文件路径

## 编写控制脚本

### 脚本结构

控制脚本必须包含两个函数：

```python
def init():
    """初始化函数，运行时启动时调用一次"""
    pass

def step():
    """周期执行函数，每个控制周期调用"""
    pass
```

### 完整示例

```python
from plcopen import PID

# 全局变量
pid = None
temperature = 20.0

def init():
    """初始化 PID 控制器"""
    global pid
    pid = PID(Kp=3.0, Ki=0.2, Kd=0.5, output_min=0, output_max=100)
    print("PID 控制器初始化完成")

def step():
    """周期控制逻辑"""
    global temperature

    # 读取传感器（示例）
    setpoint = 25.0

    # 计算控制输出
    control = pid.compute(SP=setpoint, PV=temperature)

    # 输出到执行器（示例）
    print(f"温度: {temperature:.1f}°C, 控制输出: {control:.1f}%")

    # 模拟物理过程
    temperature += (control * 0.01 - (temperature - 20.0) * 0.05)
```

### 最佳实践

1. **全局变量**: 在模块级定义功能块实例
2. **初始化**: 在 `init()` 中创建功能块
3. **周期执行**: 在 `step()` 中执行控制逻辑
4. **错误处理**: 使用 try-except 捕获异常
5. **性能优化**: 避免在 `step()` 中执行耗时操作

## 功能块参考

详细 API 参考见 [API 参考文档](api_reference.md)。

### PID 控制器

```python
from plcopen import PID

# 创建 PID
pid = PID(Kp=2.0, Ki=0.5, Kd=0.1, output_min=0, output_max=100)

# 计算输出
output = pid.compute(SP=setpoint, PV=process_value)

# 调整参数
pid.set_params(Kp=3.0, Ki=0.3)

# 重置状态
pid.reset()
```

### 一阶惯性

```python
from plcopen import FirstOrder

# 创建一阶惯性（时间常数 0.5 秒）
filter = FirstOrder(T=0.5)

# 滤波
filtered = filter.compute(input=noisy_signal)
```

### 斜率限制

```python
from plcopen import Ramp

# 创建斜率限制（上升 10 单位/秒，下降 5 单位/秒）
ramp = Ramp(rising_rate=10.0, falling_rate=5.0)

# 计算输出（dt 为时间步长）
output = ramp.compute(input=target, dt=0.1)
```

### 限幅

```python
from plcopen import Limit

# 创建限幅（范围 0-100）
limit = Limit(min_value=0.0, max_value=100.0)

# 限制输入
output = limit.compute(input=raw_value)
```

## 运行和调试

### 运行模式

1. **开发测试**: 直接运行 Python 脚本
   ```bash
   python3 my_control.py
   ```

2. **生产运行**: 使用运行时环境
   ```bash
   bin/plcopen_runtime --config config/my_config.yaml
   ```

3. **容器运行**: 使用 Docker
   ```bash
   docker run --rm -v $(pwd)/config:/app/config plcopen-runtime
   ```

### 调试技巧

1. **日志输出**: 设置 `logging.level = DEBUG`

2. **性能分析**: 查看调度器统计
   ```
   调度器统计：周期数=1000, 平均=98.5 ms, 最大=102.3 ms
   ```

3. **超时检测**: 关注超时警告
   ```
   周期超时：实际 105.2 ms > 阈值 100.0 ms
   ```

4. **Python 调试**: 使用 print() 或 logging
   ```python
   import logging
   logging.info(f"PID 输出: {output}")
   ```

## 常见问题

### 构建失败

**Q**: 编译时提示找不到 Python.h

**A**: 安装 Python 开发包：
```bash
sudo apt-get install python3-dev
```

### 运行时错误

**Q**: 提示 "调度器休眠被中断"

**A**: 正常现象，系统信号可能中断休眠。如频繁出现，检查系统负载。

### 性能问题

**Q**: 周期时间不稳定，经常超时

**A**:
1. 减少 `step()` 函数中的计算量
2. 增加 `cycle_time_ms` 配置
3. 检查系统负载（CPU、内存）
4. 考虑使用 CPU 亲和性

### PID 调优

**Q**: PID 控制输出振荡

**A**: 减小 Kp，增大 Kd。详见 [PID 温度控制示例](examples/pid_temperature.md)。

---

## 获取帮助

- **文档**: [README.md](../README.md)
- **API 参考**: [api_reference.md](api_reference.md)
- **示例**: `python/examples/`
- **问题反馈**: GitHub Issues

## 下一步

- 阅读 [API 参考文档](api_reference.md) 了解详细功能
- 查看 [PID 温度控制示例](examples/pid_temperature.md) 学习实际应用
- 参考 [常见问题](faq.md) 解决问题
