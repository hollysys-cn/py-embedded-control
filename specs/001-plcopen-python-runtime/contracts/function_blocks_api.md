# API 合约：PLCopen 功能块 Python 接口

**版本**: 1.0.0
**日期**: 2026-01-17
**协议**: Python API（基于 C Extension）

## 概述

本文档定义 PLCopen 功能块的 Python 编程接口。所有功能块类位于 `plcopen` 包中，提供统一的创建、配置和计算接口。

---

## 通用约定

### 导入方式
```python
import plcopen

# 或
from plcopen import PID, FirstOrder
```

### 参数类型
- 所有数值参数使用 Python `float`（对应 C `double`）
- 时间相关参数单位为**秒**
- 返回值为 `float` 或 `dict`

### 异常处理
- 参数验证失败：抛出 `ValueError`
- 计算错误：抛出 `RuntimeError`
- 参数超出范围：自动限制并发出 Python `Warning`（`warnings.warn()`）

---

## API 规范

### 1. PID 控制器

#### 类：`plcopen.PID`

**描述**: 标准位置式 PID 控制器。

**构造函数**:
```python
pid = plcopen.PID(
    Kp: float = 1.0,
    Ki: float = 0.0,
    Kd: float = 0.0,
    output_min: float = -1e6,
    output_max: float = 1e6
)
```

**参数**:
- `Kp`: 比例系数，默认 1.0，范围 [0, 1e6]
- `Ki`: 积分系数，默认 0.0，范围 [0, 1e6]
- `Kd`: 微分系数，默认 0.0，范围 [0, 1e6]
- `output_min`: 输出下限，默认 -1e6
- `output_max`: 输出上限，默认 1e6

**抛出**:
- `ValueError`: 如果 `output_min >= output_max`

**方法**:

#### `compute(SP: float, PV: float) -> float`

计算 PID 控制输出。

**参数**:
- `SP`: 设定值（Set Point）
- `PV`: 过程变量/反馈值（Process Variable）

**返回**:
- `float`: 控制变量（Control Variable），自动限制在 [output_min, output_max] 范围内

**示例**:
```python
pid = plcopen.PID(Kp=2.0, Ki=0.5, Kd=0.1)
control = pid.compute(SP=25.0, PV=23.5)  # 返回控制输出
print(f"控制输出: {control}")
```

#### `set_params(Kp: float = None, Ki: float = None, Kd: float = None) -> None`

运行时动态修改 PID 参数。

**参数**:
- `Kp`, `Ki`, `Kd`: 可选，仅修改提供的参数

**警告**: 如果参数超出范围，自动限制并发出警告。

**示例**:
```python
pid.set_params(Kp=3.0)  # 仅修改 Kp
```

#### `get_params() -> dict`

获取当前 PID 参数。

**返回**:
```python
{
    "Kp": float,
    "Ki": float,
    "Kd": float,
    "output_min": float,
    "output_max": float
}
```

#### `get_state() -> dict`

获取内部状态（用于诊断）。

**返回**:
```python
{
    "integral": float,      # 积分累积值
    "prev_error": float,    # 上一周期误差
    "last_error": float     # 当前误差
}
```

#### `reset() -> None`

重置 PID 内部状态（积分清零）。

---

### 2. 一阶惯性功能块

#### 类：`plcopen.FirstOrder`

**描述**: 一阶惯性环节，传递函数 H(s) = 1 / (T*s + 1)。

**构造函数**:
```python
fo = plcopen.FirstOrder(T: float = 1.0)
```

**参数**:
- `T`: 时间常数（秒），默认 1.0，范围 [0.001, 1e6]

**方法**:

#### `compute(input: float) -> float`

计算一阶惯性输出。

**参数**:
- `input`: 输入信号

**返回**:
- `float`: 输出信号

**示例**:
```python
fo = plcopen.FirstOrder(T=0.5)
output = fo.compute(input=10.0)
print(f"输出: {output}")
```

#### `set_time_constant(T: float) -> None`

动态修改时间常数。

**警告**: 如果 T 超出范围，自动限制并发出警告。

#### `get_params() -> dict`

**返回**:
```python
{"T": float}
```

#### `reset() -> None`

重置内部状态（输出清零）。

---

### 3. 斜坡功能块（Ramp）

#### 类：`plcopen.Ramp`

**描述**: 斜坡信号生成器，输出以固定速率变化。

**构造函数**:
```python
ramp = plcopen.Ramp(rate: float = 1.0)
```

**参数**:
- `rate`: 变化速率（单位/秒），默认 1.0

**方法**:

#### `compute(target: float) -> float`

计算斜坡输出。

**参数**:
- `target`: 目标值

**返回**:
- `float`: 当前输出值（逐步接近 target）

**示例**:
```python
ramp = plcopen.Ramp(rate=5.0)  # 每秒变化 5 个单位
output = ramp.compute(target=100.0)
```

---

### 4. 限幅功能块（Limit）

#### 类：`plcopen.Limit`

**描述**: 信号限幅器。

**构造函数**:
```python
limiter = plcopen.Limit(min: float = -100.0, max: float = 100.0)
```

**参数**:
- `min`: 下限
- `max`: 上限

**方法**:

#### `compute(input: float) -> float`

限制输入值到 [min, max] 范围。

**返回**:
- `float`: 限制后的值

---

## 用户脚本约定接口

用户编写的 Python 控制脚本必须实现以下两个函数：

### `init() -> None`

**描述**: 初始化函数，在运行时启动时调用一次。用于创建功能块实例和初始化变量。

**示例**:
```python
# 全局变量（在模块作用域）
pid = None
temp_setpoint = 25.0

def init():
    global pid
    pid = plcopen.PID(Kp=2.0, Ki=0.5, Kd=0.1)
    print("PID 控制器已初始化")
```

### `step() -> None`

**描述**: 周期执行函数，每个控制周期调用一次。实现控制逻辑。

**示例**:
```python
def step():
    # 读取传感器（这里用模拟值）
    current_temp = read_temperature()

    # 计算控制输出
    control = pid.compute(SP=temp_setpoint, PV=current_temp)

    # 输出到执行器
    set_heater_power(control)
```

---

## 完整示例：PID 温度控制

```python
"""
PID 温度控制示例
模拟一个加热器控制系统，目标温度 25°C
"""
import plcopen
import random

# 全局变量
pid = None
temperature = 20.0  # 初始温度
setpoint = 25.0

def init():
    """初始化 PID 控制器"""
    global pid
    pid = plcopen.PID(
        Kp=2.0,      # 比例系数
        Ki=0.5,      # 积分系数
        Kd=0.1,      # 微分系数
        output_min=0.0,    # 加热器最小功率
        output_max=100.0   # 加热器最大功率
    )
    print(f"PID 控制器已初始化，目标温度: {setpoint}°C")

def step():
    """周期执行：读取温度、计算控制、输出"""
    global temperature

    # 模拟温度变化（简化的物理模型）
    # 实际应用中替换为真实传感器读取
    control = pid.compute(SP=setpoint, PV=temperature)

    # 模拟加热效果
    heating_effect = control * 0.01  # 加热器影响
    cooling_effect = (temperature - 20.0) * 0.05  # 自然散热
    noise = random.uniform(-0.1, 0.1)  # 测量噪声

    temperature += heating_effect - cooling_effect + noise

    # 输出日志（每 10 个周期输出一次）
    if pid.get_state()["last_error"] % 10 < 0.1:
        print(f"温度: {temperature:.2f}°C, 控制输出: {control:.2f}%")
```

---

## 错误码

| 错误类型 | Python 异常 | 说明 |
|----------|-------------|------|
| 参数超出范围 | `Warning` | 自动限制，发出警告 |
| 参数类型错误 | `TypeError` | 传入非浮点数 |
| 参数逻辑错误 | `ValueError` | 如 min >= max |
| 计算错误 | `RuntimeError` | C 层计算异常 |

---

## 性能保证

- **单次计算延迟**: <1μs（PID 功能块，x86_64）
- **内存占用**: 每个功能块实例 <1KB
- **线程安全**: 不保证（单脚本运行，无并发）

---

## 版本兼容性

- **Python 版本**: 3.8+
- **向后兼容**: 遵循语义化版本规范，MAJOR 版本更新可能破坏兼容性
- **弃用策略**: 弃用功能将在下一个 MAJOR 版本移除前至少保留一个 MINOR 版本并发出 `DeprecationWarning`
