# 研究文档：PLCopen Python 运行时环境

**功能**: PLCopen Python 运行时环境
**日期**: 2026-01-17
**目的**: 解决技术实现中的关键未知项，为设计阶段提供决策依据

## 研究主题

### 1. PLCopen/IEC 61131-3 功能块标准

**问题**: PLCopen 功能块（PID、一阶惯性等）的标准定义和实现规范是什么？

**研究发现**:
- **标准参考**: IEC 61131-3 定义了 PLC 编程语言和功能块接口规范
- **PLCopen 组织**: 提供了标准功能块库（PLCopen Motion Control, Function Blocks 等）
- **PID 功能块标准接口**:
  - 输入：`SP`（设定值）、`PV`（过程变量/反馈值）、`Kp/Ki/Kd`（比例/积分/微分系数）
  - 输出：`CV`（控制变量）、`Error`（误差）
  - 算法：标准 PID 公式（位置式或增量式）
- **一阶惯性功能块**:
  - 输入：`Input`（输入信号）、`TimeConstant`（时间常数 T）
  - 输出：`Output`（输出信号）
  - 传递函数：`H(s) = 1 / (Ts + 1)`

**决策**:
- 采用 **IEC 61131-3 标准接口定义**，确保与主流 PLC 编程环境兼容
- 使用 **位置式 PID 算法**（输出绝对值），简化实现且适合大多数控制场景
- 功能块参数使用 **浮点数（float/double）** 表示，精度满足工业控制需求

**替代方案**:
- 自定义功能块接口：被拒绝，因为缺乏标准化且不利于用户迁移
- 增量式 PID：被拒绝，位置式 PID 更直观且易于调试

---

### 2. Python C Extension API 最佳实践

**问题**: 如何高效地将 C 语言功能块封装为 Python 可调用的接口？

**研究发现**:
- **Python C Extension API**: CPython 提供的官方 C API，性能最优
- **封装模式**:
  - 使用 `PyObject` 结构体封装 C 功能块实例
  - 通过 `PyTypeObject` 定义 Python 类（如 `PIDController` 类）
  - 方法映射：C 函数通过 `PyMethodDef` 数组暴露给 Python
- **内存管理**: Python GC 与 C malloc 的协同（使用 `tp_alloc/tp_dealloc`）
- **性能考量**: 直接调用 C 函数，无中间层开销，适合实时控制

**决策**:
- 采用 **Python C Extension API**（而非 ctypes 或 CFFI）
- 每个功能块类型对应一个 Python 类（如 `plcopen.PID`、`plcopen.FirstOrder`）
- 使用 `setup.py` 的 `Extension` 机制编译 C 扩展模块

**替代方案**:
- **ctypes**: 被拒绝，性能略差且需要手动管理函数签名
- **CFFI**: 被拒绝，增加额外依赖且编译配置复杂
- **pybind11**: 被拒绝，C++ 依赖不符合项目 C11 标准要求

---

### 3. Python 脚本周期性执行调度机制

**问题**: 如何实现可配置周期的 Python 脚本调度，并保证实时性？

**研究发现**:
- **调度模型选择**:
  - **方案 A**: Python `time.sleep()` 循环 → 精度受 OS 调度影响，抖动较大
  - **方案 B**: C 主循环 + `clock_nanosleep(CLOCK_MONOTONIC)` → 高精度，抖动 <1ms
  - **方案 C**: Linux `timerfd` + `epoll` → 适合事件驱动，但增加复杂度
- **Python 嵌入式调用**:
  - 使用 `Py_Initialize()` 初始化 Python 解释器
  - `PyRun_SimpleString()` 或 `PyImport_ImportModule()` 加载用户脚本
  - 周期内调用脚本的 `step()` 函数（约定接口）
- **超时检测**: 记录每次循环的开始时间，检测执行时间是否超过周期

**决策**:
- 采用 **方案 B**：C 主循环 + `clock_nanosleep`，Python 作为嵌入式脚本引擎
- 用户脚本必须实现 `init()` 和 `step()` 函数（约定接口）
- 周期时间可配置（配置文件或命令行参数），默认 100ms
- 超时阈值为周期的 110%，超时时记录警告日志但继续执行

**替代方案**:
- Python asyncio 事件循环：被拒绝，增加复杂度且不适合严格周期控制
- 实时 Linux 内核补丁（PREEMPT_RT）：暂不考虑，标准 Linux 内核足够满足需求

---

### 4. 远程调试协议选型（debugpy vs pdb）

**问题**: 使用哪种 Python 调试协议实现远程调试功能？

**研究发现**:
- **debugpy**:
  - 基于 Debug Adapter Protocol (DAP)，VS Code 原生支持
  - 支持网络远程调试（TCP 端口）
  - 功能丰富：断点、单步执行、变量查看、表达式求值
  - 依赖：`pip install debugpy`（MIT License）
- **pdb**:
  - Python 标准库，无需额外依赖
  - 命令行界面，远程调试需要通过 SSH 或 socket 包装
  - 功能基础：断点、单步，但无图形化支持

**决策**:
- 采用 **debugpy**，理由：
  - VS Code 集成度高，用户体验好
  - DAP 协议标准化，未来可支持其他编辑器（PyCharm、Vim 等）
  - 远程调试开箱即用，无需额外封装
- 调试服务器在运行时可选启动（通过配置文件或命令行参数 `--debug-port`）
- 默认监听 `0.0.0.0:5678`（debugpy 默认端口）

**替代方案**:
- pdb：被拒绝，远程调试体验差且缺乏图形化支持
- 自定义调试协议：被拒绝，重复造轮子且维护成本高

---

### 5. 功能块参数验证与限制机制

**问题**: 如何实现参数自动验证和限制（clamp）功能？

**研究发现**:
- **验证时机**: 参数设置时（Python API 调用 `set_param()` 方法）
- **验证逻辑**:
  - 每个功能块定义参数有效范围（min/max 值）
  - 超出范围时自动限制到边界值（`clamp(value, min, max)`）
  - 记录警告日志（包含参数名、原始值、限制后的值）
- **参数范围示例**:
  - PID Kp/Ki/Kd：`[0, 1e6]`（非负数，上限避免数值溢出）
  - 一阶惯性时间常数 T：`[0.001, 1e6]`（避免除零和极端值）

**决策**:
- 在 C 层实现参数验证函数（`validate_and_clamp()`）
- Python 绑定层调用验证函数，并通过 Python 警告系统（`warnings.warn()`）通知用户
- 日志系统记录所有参数限制事件（日志级别：WARNING）

**替代方案**:
- 抛出异常拒绝非法参数：被拒绝，影响控制连续性
- 静默接受任何值：被拒绝，可能导致不可预测行为

---

### 6. 日志系统设计

**问题**: 如何实现满足需求的日志记录功能？

**研究发现**:
- **日志需求**:
  - 记录功能块调用、参数变化、异常错误、超时警告
  - 简体中文日志消息
  - 支持日志级别（DEBUG、INFO、WARNING、ERROR）
- **实现选择**:
  - **C 层**: 使用 `syslog()`（Linux 标准）或自定义日志函数
  - **Python 层**: 使用 Python `logging` 模块
  - **统一输出**: C 和 Python 日志合并到同一目标（文件或 syslog）

**决策**:
- C 层使用 **简化的文件日志**（`fprintf` 到 `runtime.log`），避免 syslog 配置复杂度
- Python 层使用 `logging` 模块，配置为输出到同一日志文件
- 日志格式：`[时间戳] [级别] [模块] 消息`（简体中文）
- 支持通过配置文件设置日志级别和输出路径

**替代方案**:
- 使用第三方 C 日志库（如 spdlog）：被拒绝，增加依赖且过度设计
- 仅使用 Python logging：被拒绝，C 层需要独立日志能力

---

## 技术栈最终决策总结

| 层次 | 技术选型 | 理由 |
|------|----------|------|
| 功能块核心 | C11 | 高性能、IEC 61131-3 兼容 |
| Python 绑定 | CPython C Extension API | 官方支持、零依赖、最佳性能 |
| 调度器 | C 主循环 + clock_nanosleep | 高精度、低抖动 |
| 远程调试 | debugpy (DAP) | VS Code 集成、标准协议 |
| 日志系统 | 文件日志（C fprintf + Python logging） | 简单可靠、统一输出 |
| 构建系统 | Makefile + setup.py | 标准工具链、易于集成 |
| 测试框架 | pytest (Python) + 自定义单元测试（C） | 行业标准 |

## 遗留未决问题

无。所有关键技术选型已完成决策。
