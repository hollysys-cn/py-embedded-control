# VS Code 调试截图说明

本文档说明调试功能的各种场景和预期的 UI 表现。

## 1. 断点设置

**操作**：点击行号左侧设置断点

**预期表现**：
- 未激活：灰色空心圆（调试器未连接）
- 已激活：红色实心圆（调试器已连接）

**示例代码位置**：
```python
# python/examples/pid_temperature_control.py

def step():
    """每个控制周期执行"""
    current_temp = fb.get("temperature_pid", "process_value")  # <-- 在此行设置断点
    output = fb.compute("temperature_pid", current_temp)
    fb.set("temperature_pid", "process_value", current_temp)
```

**关键要素**：
- 文件路径：`python/examples/pid_temperature_control.py`
- 行号：`step()` 函数内第一行
- 断点类型：普通断点（无条件）

---

## 2. 变量查看

**操作**：程序在断点处暂停时，查看"变量"面板

**预期显示的变量**：

### 局部变量（Locals）
```
current_temp: 72.5
output: 45.8
error: -2.5
```

### 全局变量（Globals）
```
fb: <module 'plcopen.fb' ...>
__name__: "__main__"
__file__: "/workspace/python/examples/pid_temperature_control.py"
```

### 功能块状态（通过监视表达式）
```
fb.state["temperature_pid"]["setpoint"]: 75.0
fb.state["temperature_pid"]["kp"]: 2.0
fb.state["temperature_pid"]["ki"]: 0.5
fb.state["temperature_pid"]["kd"]: 0.1
fb.state["temperature_pid"]["integral"]: 3.2
fb.state["temperature_pid"]["last_error"]: -2.8
```

**关键要素**：
- 面板位置：左侧"运行和调试"视图 → "变量"部分
- 变量类型：局部变量、全局变量、模块属性
- 可编辑：双击变量值可以临时修改（仅当前调试会话有效）

---

## 3. 单步执行

**操作**：使用调试工具栏按钮

**按钮说明**：
- **继续 (F5)**: 继续执行到下一个断点
- **单步跳过 (F10)**: 执行当前行，不进入函数
- **单步进入 (F11)**: 进入函数内部
- **单步跳出 (Shift+F11)**: 跳出当前函数

**执行流程示例**：

### 起始状态（断点暂停）
```python
def step():
    current_temp = fb.get("temperature_pid", "process_value")  # <-- 当前行
    output = fb.compute("temperature_pid", current_temp)
    fb.set("temperature_pid", "process_value", current_temp)
```

### 按 F10（单步跳过）
```python
def step():
    current_temp = fb.get("temperature_pid", "process_value")  # 已执行
    output = fb.compute("temperature_pid", current_temp)  # <-- 当前行
    fb.set("temperature_pid", "process_value", current_temp)
```

### 按 F11（单步进入）
```python
# 进入 fb.compute() 函数内部（在 plcopen/fb.py 中）
def compute(name, input_value):
    """执行功能块计算"""
    state = fb_state.get(name)  # <-- 当前行（进入函数内部）
    if not state:
        return 0.0
```

**关键要素**：
- 当前行：黄色高亮（箭头指示）
- 调用堆栈：显示函数调用链（`step() → compute() → pid_compute()`）
- 可随时查看变量值变化

---

## 4. 调试控制台

**操作**：在"调试控制台"面板输入 Python 表达式

**示例交互**：

```python
>>> current_temp
72.5

>>> output
45.8

>>> fb.get("temperature_pid", "setpoint")
75.0

>>> fb.state
{'temperature_pid': {'type': 'pid', 'kp': 2.0, 'ki': 0.5, ...}}

>>> error = current_temp - fb.get("temperature_pid", "setpoint")
>>> error
-2.5

>>> # 临时修改变量（仅当前会话）
>>> current_temp = 100.0
>>> current_temp
100.0
```

**关键要素**：
- 面板位置：底部面板 → "调试控制台"选项卡
- 提示符：`>>>` 表示等待输入
- 实时求值：可以评估任意 Python 表达式
- 变量修改：临时修改，仅影响当前调试会话

---

## 5. 调用堆栈

**操作**：程序在断点处暂停时，查看"调用堆栈"面板

**预期显示**：

```
step (pid_temperature_control.py:15)
  └─ compute (fb.py:42)
      └─ pid_compute (fb.py:87)
          └─ _calculate_pid_output (fb.py:102)  # <-- 当前栈帧
```

**堆栈详情**：
```
#0  _calculate_pid_output  pid_temperature_control.py:102
    local: proportional = 5.0, integral = 1.6, derivative = 0.05

#1  pid_compute  pid_temperature_control.py:87
    local: error = -2.5, dt = 0.1

#2  compute  pid_temperature_control.py:42
    local: name = "temperature_pid", input_value = 72.5

#3  step  pid_temperature_control.py:15
    local: current_temp = 72.5
```

**关键要素**：
- 面板位置：左侧"运行和调试"视图 → "调用堆栈"部分
- 栈顶：当前执行位置（#0）
- 点击栈帧：切换查看不同栈帧的变量和代码

---

## 6. 监视表达式

**操作**：在"监视"面板添加自定义表达式

**常用监视表达式**：

```python
# PID 控制器关键状态
fb.get("temperature_pid", "process_value")     # 当前温度
fb.get("temperature_pid", "setpoint")          # 目标温度
fb.get("temperature_pid", "error")             # 偏差
fb.get("temperature_pid", "integral")          # 积分项
fb.get("temperature_pid", "derivative")        # 微分项
fb.get("temperature_pid", "output")            # 控制输出

# 计算的表达式
current_temp - fb.get("temperature_pid", "setpoint")  # 实时偏差
abs(fb.get("temperature_pid", "error")) < 1.0         # 是否在稳态（布尔值）
```

**预期显示**：
```
监视
  ▼ fb.get("temperature_pid", "process_value")
      72.5
  ▼ fb.get("temperature_pid", "setpoint")
      75.0
  ▼ current_temp - fb.get("temperature_pid", "setpoint")
      -2.5
  ▼ abs(fb.get("temperature_pid", "error")) < 1.0
      False
```

**关键要素**：
- 面板位置：左侧"运行和调试"视图 → "监视"部分
- 实时更新：每次暂停时自动重新评估
- 支持复杂表达式：任意 Python 代码

---

## 7. 条件断点

**操作**：右键断点 → "编辑断点" → 选择"表达式"或"命中次数"

**示例：表达式条件**

```python
# 仅当温度超过 90°C 时暂停
current_temp > 90.0

# 仅当输出异常时暂停
output < 0 or output > 100

# 仅当偏差较大时暂停
abs(error) > 10.0
```

**示例：命中次数**

```
# 每 10 次循环暂停一次
% 10 == 0

# 前 5 次不暂停，从第 6 次开始暂停
>= 5
```

**关键要素**：
- 断点样式：带条件的断点显示为红色圆点加白色对勾
- 条件评估：每次执行到该行时评估条件，仅满足条件时暂停
- 性能影响：条件断点会增加轻微开销

---

## 8. 日志断点

**操作**：右键断点 → "编辑断点" → 选择"日志消息"

**示例日志消息**：

```python
# 打印变量值（使用花括号）
"温度: {current_temp}, 输出: {output}"

# 打印复杂表达式
"偏差: {current_temp - fb.get('temperature_pid', 'setpoint')}"

# 打印时间戳
"[{datetime.now()}] 周期执行"
```

**预期输出**（调试控制台）：

```
温度: 72.5, 输出: 45.8
温度: 72.8, 输出: 46.2
温度: 73.1, 输出: 46.5
偏差: -2.5
偏差: -2.2
偏差: -1.9
```

**关键要素**：
- 不暂停程序：仅打印日志消息，程序继续执行
- 性能友好：比频繁使用断点更高效
- 输出位置：调试控制台

---

## 实际使用建议

### 初次调试

1. 在 `step()` 函数第一行设置断点
2. 启动运行时 + 附加调试器
3. 观察变量面板，了解数据结构
4. 使用 F10 单步跳过，逐行执行
5. 在调试控制台实验表达式

### 问题诊断

1. 使用条件断点，仅在异常情况下暂停（例如 `output < 0`）
2. 添加监视表达式，实时追踪关键变量
3. 查看调用堆栈，定位问题源头
4. 修改变量值，测试不同场景

### 性能分析

1. 使用日志断点代替普通断点（避免频繁暂停）
2. 记录每个周期的关键数据
3. 在调试控制台使用 `timeit` 分析耗时
4. 查看调用堆栈深度，识别递归或深层调用

---

## 调试配置示例

### launch.json（本地调试）

```json
{
    "name": "Python: 附加到运行时",
    "type": "debugpy",
    "request": "attach",
    "connect": {
        "host": "localhost",
        "port": 5678
    },
    "pathMappings": [
        {
            "localRoot": "${workspaceFolder}/python",
            "remoteRoot": "${workspaceFolder}/python"
        }
    ],
    "justMyCode": false
}
```

### 运行时配置（启用调试）

```yaml
# config/pid_temperature_debug.yaml
debug:
  enabled: true
  host: "0.0.0.0"
  port: 5678
  timeout: 10
```

---

## 故障排除

### 问题：断点未激活（灰色圆圈）

**原因**：路径映射不正确

**解决方法**：
1. 检查 `launch.json` 中的 `localRoot` 和 `remoteRoot`
2. 确保路径与实际文件位置匹配
3. 使用绝对路径或 `${workspaceFolder}` 变量

### 问题：无法连接到 debugpy

**原因**：端口未监听或防火墙阻止

**解决方法**：
1. 确认运行时已启动：`ps aux | grep plcopen_runtime`
2. 检查端口监听：`netstat -tuln | grep 5678`
3. Docker 环境：确保端口已映射（`-p 5678:5678`）
4. 检查防火墙设置

### 问题：调试导致周期超时

**原因**：断点暂停导致控制周期无法按时完成

**解决方法**：
1. 增加周期时间（`cycle_period_ms: 1000`）
2. 使用条件断点减少暂停频率
3. 使用日志断点代替普通断点
4. 仅在开发环境调试，生产环境禁用

---

## 总结

调试功能提供了强大的工具来理解和诊断 Python 控制脚本：

✅ **断点设置**：精确控制程序暂停位置
✅ **变量查看**：实时观察数据变化
✅ **单步执行**：逐行跟踪代码执行
✅ **调试控制台**：交互式实验表达式
✅ **调用堆栈**：理解函数调用关系
✅ **监视表达式**：持续追踪关键数据
✅ **条件断点**：仅在异常情况下暂停
✅ **日志断点**：不暂停程序的日志输出

通过合理使用这些功能，可以大幅提高开发效率和代码质量。
