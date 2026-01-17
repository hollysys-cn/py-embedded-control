# PLCopen Python 运行时环境 - API 参考

## 目录

1. [功能块 API](#功能块-api)
   - [PID 控制器](#pid-控制器)
   - [一阶惯性滤波](#一阶惯性滤波)
   - [斜率限制](#斜率限制)
   - [限幅](#限幅)
2. [Python 模块 API](#python-模块-api)
3. [运行时 API](#运行时-api)
4. [配置 API](#配置-api)

---

## 功能块 API

所有功能块从 `plcopen` 模块导入：

```python
from plcopen import PID, FirstOrder, Ramp, Limit
```

### PID 控制器

#### 类: `PID`

实现 PLCopen 标准的 PID 控制器。

##### 构造函数

```python
PID(Kp=1.0, Ki=0.0, Kd=0.0, output_min=0.0, output_max=100.0)
```

**参数:**
- `Kp` (float): 比例增益，默认 1.0
- `Ki` (float): 积分增益，默认 0.0
- `Kd` (float): 微分增益，默认 0.0
- `output_min` (float): 输出最小值，默认 0.0
- `output_max` (float): 输出最大值，默认 100.0

**返回:**
- PID 控制器实例

**示例:**
```python
pid = PID(Kp=2.0, Ki=0.5, Kd=0.1, output_min=-100, output_max=100)
```

##### 方法: `compute`

```python
compute(SP, PV) -> float
```

计算 PID 输出。

**参数:**
- `SP` (float): 设定值（Setpoint）
- `PV` (float): 过程值（Process Value）

**返回:**
- float: 控制输出（经过限幅）

**示例:**
```python
output = pid.compute(SP=25.0, PV=23.5)
```

##### 方法: `set_params`

```python
set_params(Kp=None, Ki=None, Kd=None, output_min=None, output_max=None)
```

更新 PID 参数（可选参数）。

**参数:**
- `Kp` (float, optional): 新的比例增益
- `Ki` (float, optional): 新的积分增益
- `Kd` (float, optional): 新的微分增益
- `output_min` (float, optional): 新的输出最小值
- `output_max` (float, optional): 新的输出最大值

**示例:**
```python
pid.set_params(Kp=3.0, Ki=0.3)  # 仅更新 Kp 和 Ki
```

##### 方法: `reset`

```python
reset()
```

重置 PID 内部状态（积分和微分状态）。

**示例:**
```python
pid.reset()
```

##### 算法说明

PID 控制器使用以下公式：

```
error = SP - PV
integral += error * dt
derivative = (error - prev_error) / dt
output = Kp * error + Ki * integral + Kd * derivative
output = clamp(output, output_min, output_max)
```

---

### 一阶惯性滤波

#### 类: `FirstOrder`

实现一阶惯性环节，用于信号滤波和动态建模。

##### 构造函数

```python
FirstOrder(T=1.0)
```

**参数:**
- `T` (float): 时间常数（秒），默认 1.0

**返回:**
- FirstOrder 滤波器实例

**示例:**
```python
filter = FirstOrder(T=0.5)  # 时间常数 0.5 秒
```

##### 方法: `compute`

```python
compute(input) -> float
```

计算滤波输出。

**参数:**
- `input` (float): 输入值

**返回:**
- float: 滤波后的输出

**示例:**
```python
filtered = filter.compute(input=noisy_signal)
```

##### 方法: `set_time_constant`

```python
set_time_constant(T)
```

更新时间常数。

**参数:**
- `T` (float): 新的时间常数（秒）

**示例:**
```python
filter.set_time_constant(T=1.0)
```

##### 方法: `reset`

```python
reset()
```

重置内部状态。

**示例:**
```python
filter.reset()
```

##### 算法说明

一阶惯性环节使用以下传递函数：

```
G(s) = 1 / (T*s + 1)
```

离散化实现（欧拉法）：

```
output = output + (input - output) * dt / T
```

---

### 斜率限制

#### 类: `Ramp`

限制信号变化率，防止输出突变。

##### 构造函数

```python
Ramp(rising_rate=1.0, falling_rate=1.0)
```

**参数:**
- `rising_rate` (float): 上升斜率（单位/秒），默认 1.0
- `falling_rate` (float): 下降斜率（单位/秒），默认 1.0

**返回:**
- Ramp 实例

**示例:**
```python
ramp = Ramp(rising_rate=10.0, falling_rate=5.0)
```

##### 方法: `compute`

```python
compute(input, dt) -> float
```

计算斜率限制后的输出。

**参数:**
- `input` (float): 输入值
- `dt` (float): 时间步长（秒）

**返回:**
- float: 斜率限制后的输出

**示例:**
```python
output = ramp.compute(input=target, dt=0.1)
```

##### 方法: `set_rates`

```python
set_rates(rising_rate=None, falling_rate=None)
```

更新斜率参数。

**参数:**
- `rising_rate` (float, optional): 新的上升斜率
- `falling_rate` (float, optional): 新的下降斜率

**示例:**
```python
ramp.set_rates(rising_rate=20.0)  # 仅更新上升斜率
```

##### 方法: `reset`

```python
reset()
```

重置内部状态。

**示例:**
```python
ramp.reset()
```

##### 算法说明

斜率限制算法：

```
delta = input - prev_output
max_change = (rising_rate if delta > 0 else falling_rate) * dt
output = prev_output + clamp(delta, -max_change, max_change)
```

---

### 限幅

#### 类: `Limit`

将输入限制在指定范围内。

##### 构造函数

```python
Limit(min_value=0.0, max_value=100.0)
```

**参数:**
- `min_value` (float): 最小值，默认 0.0
- `max_value` (float): 最大值，默认 100.0

**返回:**
- Limit 实例

**示例:**
```python
limit = Limit(min_value=-50.0, max_value=50.0)
```

##### 方法: `compute`

```python
compute(input) -> float
```

计算限幅后的输出。

**参数:**
- `input` (float): 输入值

**返回:**
- float: 限幅后的输出

**示例:**
```python
output = limit.compute(input=raw_value)
```

##### 方法: `set_limits`

```python
set_limits(min_value=None, max_value=None)
```

更新限幅范围。

**参数:**
- `min_value` (float, optional): 新的最小值
- `max_value` (float, optional): 新的最大值

**示例:**
```python
limit.set_limits(min_value=-100.0, max_value=100.0)
```

##### 算法说明

限幅算法：

```
output = clamp(input, min_value, max_value)
```

---

## Python 模块 API

### 脚本接口

控制脚本必须实现以下接口：

#### 函数: `init`

```python
def init():
    """
    初始化函数，运行时启动时调用一次。
    用于创建功能块实例、初始化变量等。
    """
    pass
```

**调用时机:** 运行时启动时，在第一个控制周期前

**用途:**
- 创建功能块实例
- 初始化全局变量
- 加载配置参数
- 建立外部连接

**示例:**
```python
from plcopen import PID

pid = None

def init():
    global pid
    pid = PID(Kp=2.0, Ki=0.5, Kd=0.1)
    print("初始化完成")
```

#### 函数: `step`

```python
def step():
    """
    周期执行函数，每个控制周期调用一次。
    实现控制逻辑。
    """
    pass
```

**调用时机:** 每个控制周期

**用途:**
- 读取传感器数据
- 执行控制算法
- 输出到执行器
- 记录数据

**示例:**
```python
def step():
    setpoint = 25.0
    pv = read_sensor()
    output = pid.compute(SP=setpoint, PV=pv)
    write_actuator(output)
```

---

## 运行时 API

运行时可执行文件：`bin/plcopen_runtime`

### 命令行选项

```bash
bin/plcopen_runtime [OPTIONS]
```

#### `--config <file>`

指定配置文件路径。

**示例:**
```bash
bin/plcopen_runtime --config config/my_config.yaml
```

#### `--help`

显示帮助信息。

**示例:**
```bash
bin/plcopen_runtime --help
```

### 信号处理

运行时处理以下信号：

- **SIGINT** (Ctrl+C): 优雅关闭
- **SIGTERM**: 优雅关闭

---

## 配置 API

配置文件格式：YAML

### 完整配置示例

```yaml
# 日志配置
logging:
  level: INFO              # DEBUG | INFO | WARNING | ERROR
  file: logs/runtime.log   # 日志文件路径
  max_size: 1048576        # 最大文件大小（字节）

# 调度器配置
scheduler:
  cycle_time_ms: 100       # 控制周期（毫秒）
  timeout_ms: 50           # 超时阈值（毫秒）
  max_jitter_ms: 5         # 最大抖动（毫秒）

# Python 脚本配置
script:
  path: python/examples/pid_temperature.py  # 脚本路径
```

### 配置项说明

#### `logging.level`

**类型:** string

**可选值:** DEBUG, INFO, WARNING, ERROR

**默认值:** INFO

**说明:** 日志输出级别

---

#### `logging.file`

**类型:** string

**默认值:** logs/runtime.log

**说明:** 日志文件路径（相对或绝对）

---

#### `logging.max_size`

**类型:** integer

**默认值:** 1048576 (1MB)

**说明:** 日志文件最大大小（字节）

---

#### `scheduler.cycle_time_ms`

**类型:** integer

**默认值:** 100

**范围:** 10-10000

**说明:** 控制周期（毫秒）

---

#### `scheduler.timeout_ms`

**类型:** integer

**默认值:** cycle_time_ms * 0.5

**说明:** 超时检测阈值（毫秒）

---

#### `scheduler.max_jitter_ms`

**类型:** integer

**默认值:** cycle_time_ms * 0.05

**说明:** 最大允许抖动（毫秒）

---

#### `script.path`

**类型:** string

**必需:** 是

**说明:** Python 控制脚本路径

---

## 错误代码

| 代码 | 说明 |
|------|------|
| 0 | 成功 |
| 1 | 配置文件错误 |
| 2 | Python 初始化失败 |
| 3 | 脚本加载失败 |
| 4 | 运行时错误 |

---

## 版本信息

- **API 版本:** 1.0.0
- **运行时版本:** 1.0.0
- **PLCopen 兼容:** 部分兼容 PLCopen Motion Control Part 1

---

## 性能规格

| 指标 | 值 |
|------|------|
| 最小周期时间 | 10ms |
| 最大周期时间 | 10s |
| 调度精度 | ±1ms |
| 最大功能块数 | 无限制（受内存限制） |
| Python 调用开销 | ~0.1ms |

---

## 下一步

- 阅读 [用户手册](user_guide.md) 了解使用方法
- 查看 [示例文档](examples/pid_temperature.md) 学习实际应用
- 参考 [常见问题](faq.md) 解决问题
