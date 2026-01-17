# 数据模型：PLCopen Python 运行时环境

**功能**: PLCopen Python 运行时环境
**日期**: 2026-01-17
**目的**: 定义系统核心实体及其关系，为实现阶段提供结构化数据设计

## 实体定义

### 1. FunctionBlock（功能块）

**描述**: PLCopen 标准功能块的抽象基础结构，所有具体功能块继承此结构。

**属性**:
- `id` (string): 功能块实例唯一标识符，由 Python 层生成
- `type` (enum): 功能块类型（PID, FIRST_ORDER, RAMP, LIMIT）
- `state` (struct): 功能块内部状态（依类型而异）
- `params` (struct): 功能块参数（依类型而异）
- `inputs` (struct): 当前周期输入值
- `outputs` (struct): 当前周期输出值
- `last_update_time` (double): 上次更新时间戳（秒，CLOCK_MONOTONIC）

**关系**:
- 一对多：一个 RuntimeContext 包含多个 FunctionBlock 实例
- 无持久化需求（内存中管理，脚本重启时重建）

**状态机**: 无（功能块无生命周期状态，始终处于可计算状态）

---

### 2. PIDFunctionBlock（PID 功能块）

**继承**: FunctionBlock

**描述**: PID 控制器功能块，实现标准位置式 PID 算法。

**参数** (`params`):
- `Kp` (double): 比例系数，范围 [0, 1e6]，默认 1.0
- `Ki` (double): 积分系数，范围 [0, 1e6]，默认 0.0
- `Kd` (double): 微分系数，范围 [0, 1e6]，默认 0.0
- `output_min` (double): 输出下限，默认 -1e6
- `output_max` (double): 输出上限，默认 1e6

**输入** (`inputs`):
- `SP` (double): 设定值（Set Point）
- `PV` (double): 过程变量/反馈值（Process Variable）

**输出** (`outputs`):
- `CV` (double): 控制变量（Control Variable）
- `Error` (double): 当前误差（SP - PV）

**内部状态** (`state`):
- `integral` (double): 积分累积值
- `prev_error` (double): 上一周期误差（用于微分计算）

**验证规则**:
- Kp/Ki/Kd 自动限制到 [0, 1e6]，超出时记录警告
- output_min < output_max，否则抛出错误

---

### 3. FirstOrderFunctionBlock（一阶惯性功能块）

**继承**: FunctionBlock

**描述**: 一阶惯性环节功能块，传递函数 H(s) = 1 / (T*s + 1)。

**参数** (`params`):
- `T` (double): 时间常数（秒），范围 [0.001, 1e6]，默认 1.0

**输入** (`inputs`):
- `Input` (double): 输入信号

**输出** (`outputs`):
- `Output` (double): 输出信号

**内部状态** (`state`):
- `prev_output` (double): 上一周期输出值

**算法**: 离散化公式（双线性变换）:
```
dt = 当前时间 - 上次更新时间
alpha = dt / (T + dt)
Output = alpha * Input + (1 - alpha) * prev_output
```

---

### 4. RuntimeContext（运行时上下文）

**描述**: 运行时环境的全局状态管理结构。

**属性**:
- `config` (struct): 配置参数（从配置文件加载）
  - `cycle_period_ms` (int): 控制周期（毫秒），默认 100
  - `script_path` (string): Python 脚本路径
  - `debug_port` (int): debugpy 端口，0 表示禁用，默认 0
  - `log_level` (enum): 日志级别（DEBUG, INFO, WARNING, ERROR）
  - `log_file` (string): 日志文件路径，默认 "runtime.log"
- `py_interpreter` (PyObject*): Python 解释器实例
- `user_script` (PyObject*): 用户脚本模块对象
- `function_blocks` (array): 功能块实例数组（最大 32 个实例）
- `cycle_count` (uint64_t): 执行周期计数器
- `timeout_count` (uint64_t): 超时警告计数器

**关系**:
- 一对多：一个 RuntimeContext 包含多个 FunctionBlock
- 单例模式：全局唯一 RuntimeContext 实例

**生命周期**:
1. `init()`: 加载配置、初始化 Python 解释器、加载用户脚本
2. `run()`: 主循环，周期性调用脚本 `step()` 函数
3. `cleanup()`: 释放资源、关闭 Python 解释器

---

### 5. DebugSession（调试会话）

**描述**: 远程调试会话状态（由 debugpy 管理，本系统仅记录状态）。

**属性**:
- `enabled` (bool): 调试是否启用
- `port` (int): 监听端口
- `connected` (bool): 客户端是否已连接

**关系**:
- 独立于 RuntimeContext，调试会话中断不影响控制逻辑

**行为**:
- debugpy 在单独线程中运行，不阻塞主循环
- 连接状态通过 debugpy API 查询

---

## 数据流图

```
┌─────────────────────────────────────────────────────────────┐
│                      RuntimeContext                          │
│  ┌──────────────┐   ┌──────────────┐   ┌──────────────┐   │
│  │ Config       │   │ FunctionBlock│   │ FunctionBlock│   │
│  │  • 周期      │   │   • PID #1   │   │  • 一阶 #2   │   │
│  │  • 脚本路径  │   │   • 状态     │   │  • 状态      │   │
│  └──────────────┘   │   • 输入/输出│   │  • 输入/输出 │   │
│         ▲           └──────────────┘   └──────────────┘   │
│         │                  ▲                  ▲            │
│         │                  │                  │            │
│    加载配置               更新状态           更新状态       │
│         │                  │                  │            │
└─────────┼──────────────────┼──────────────────┼────────────┘
          │                  │                  │
          ▼                  │                  │
   ┌────────────┐           │                  │
   │ 配置文件   │           │                  │
   │ (YAML)     │           │                  │
   └────────────┘           │                  │
                             │                  │
                 ┌───────────┴──────────────────┴──────────┐
                 │        Python 用户脚本                   │
                 │  def init():                             │
                 │      # 创建功能块实例                    │
                 │      pid = plcopen.PID(Kp=1.0, ...)     │
                 │                                          │
                 │  def step():                             │
                 │      # 读取传感器（模拟）                │
                 │      temp = read_temperature()          │
                 │      # 调用功能块                        │
                 │      control = pid.compute(SP=25, PV=temp)│
                 │      # 输出控制信号                      │
                 │      set_heater(control)                 │
                 └─────────────────────────────────────────┘
```

## 参数验证规则汇总

| 功能块 | 参数 | 有效范围 | 默认值 | 超范围处理 |
|--------|------|----------|--------|------------|
| PID | Kp | [0, 1e6] | 1.0 | clamp + 警告 |
| PID | Ki | [0, 1e6] | 0.0 | clamp + 警告 |
| PID | Kd | [0, 1e6] | 0.0 | clamp + 警告 |
| PID | output_min | (-∞, output_max) | -1e6 | 抛出错误 |
| PID | output_max | (output_min, +∞) | 1e6 | 抛出错误 |
| FirstOrder | T | [0.001, 1e6] | 1.0 | clamp + 警告 |

## 持久化需求

**无持久化需求**。所有数据在内存中管理：
- 功能块状态在脚本重启时丢失（符合澄清决策）
- 配置通过文件加载（启动时读取，运行时不写回）
- 日志持久化到文件（仅用于诊断，不影响功能）

## 并发控制

**无并发问题**：
- 单脚本运行，无多线程访问功能块状态
- debugpy 在独立线程中运行，但不访问功能块数据
- 无需互斥锁或其他同步机制
