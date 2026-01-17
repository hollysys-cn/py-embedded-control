# 实施计划：PLCopen Python 运行时环境

**Branch**: `001-plcopen-python-runtime` | **Date**: 2026-01-17 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-plcopen-python-runtime/spec.md`

**Note**: 本文档由 `/speckit.plan` 命令生成。

## 摘要

本功能旨在构建一个运行在 Linux 嵌入式环境的 PLCopen 标准功能块运行时系统。系统核心采用 C11 语言实现高性能的控制算法功能块（PID、一阶惯性等），通过 Python C Extension API 为 Python 3.8+ 提供绑定接口。用户可使用 Python 脚本快速组合功能块实现控制逻辑（如 PID 温度控制），并通过 debugpy 协议远程调试。系统采用单脚本周期性执行模型，功能块参数动态可调且自动验证，日志系统记录运行状态和异常。技术路线遵循 YAGNI 原则，优先实现 MVP 核心功能。

## 技术上下文

**Language/Version**: C11 (功能块核心) + Python 3.8+ (脚本接口)
**Primary Dependencies**:
  - CPython C Extension API (Python 绑定)
  - debugpy (远程调试)
  - 无第三方 C 库依赖（功能块纯C实现）
**Storage**: 配置文件（YAML/INI）用于运行时参数，无需数据库
**Testing**: pytest (Python 层)，cppcheck + 自定义单元测试框架 (C 层)
**Target Platform**: Linux (Ubuntu 20.04+/Debian 10+)，支持 x86_64 和 ARM (Cortex-A7+)
**Project Type**: single (嵌入式运行时程序)
**Performance Goals**:
  - 控制周期稳定性 95%（10ms-1000ms 范围）
  - 支持 10+ 功能块实例，CPU 占用 <50%（ARM Cortex-A7）
  - PID 算法精度误差 <0.1%
**Constraints**:
  - 内存占用 <100MB（含 Python 解释器）
  - 24 小时运行内存增长 <10%（无泄漏）
  - 调试连接建立 <5s，断点响应 <1s
**Scale/Scope**:
  - 4-6 种基础功能块（PID、一阶惯性、斜坡、限幅等）
  - 单脚本运行，约 200-500 行 C 代码/功能块
  - 1 个温度控制示例，文档 <100 页

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

基于 `.specify/memory/constitution.md` 的核心原则进行检查：

- [x] **简体中文优先**：所有文档、注释（C/Python）、日志消息使用简体中文 ✅
- [x] **SOLID 原则**：C 功能块遵循单一职责（每个 .c 文件一个功能块），Python 绑定通过接口隔离 ✅
- [x] **架构兼容性**：新项目，无现有架构约束。初期建立清晰目录结构 ✅
- [x] **环境隔离**：使用 Python venv，项目级 .env 配置，不污染系统环境 ✅
- [x] **编程语言标准**：C11 标准（`gcc -std=c11`），Python 遵循 PEP 8/484 ✅
- [x] **依赖管理**：debugpy 从阿里云 PyPI 镜像安装，版本锁定在 requirements.txt ✅
- [x] **许可合规**：debugpy (MIT), CPython (PSF License - 商业友好) ✅
- [x] **代码质量**：简体中文注释，使用 cppcheck (C) + pylint/flake8 (Python)，函数圈复杂度 ≤10 ✅
- [x] **修改范围**：新项目，无修改范围问题 ✅
- [x] **文件编码**：UTF-8 无 BOM，LF 行尾，.gitattributes 强制 ✅
- [x] **质量门禁**：设计阶段需通过架构评审，实现阶段需通过静态分析和单元测试 ✅

**评审结果**：✅ 通过所有宪章原则检查，无违规项。

## Project Structure

### Documentation (this feature)

```text
specs/001-plcopen-python-runtime/
├── spec.md              # 功能规格说明（已完成）
├── plan.md              # 本文件（实施计划）
├── research.md          # Phase 0 研究文档（待生成）
├── data-model.md        # Phase 1 数据模型（待生成）
├── quickstart.md        # Phase 1 快速入门（待生成）
├── contracts/           # Phase 1 API 合约（待生成）
│   └── function_blocks_api.md
└── tasks.md             # Phase 2 任务列表（/speckit.tasks 命令生成）
```

### Source Code (repository root)

```text
py-embedded-control/
├── src/                           # C 语言源代码目录
│   ├── runtime/                   # 运行时核心
│   │   ├── main.c                 # 主程序入口
│   │   ├── scheduler.c/h          # 周期调度器
│   │   ├── py_embed.c/h           # Python 嵌入接口
│   │   └── logger.c/h             # 日志系统
│   ├── function_blocks/           # PLCopen 功能块实现
│   │   ├── fb_common.h            # 功能块通用定义
│   │   ├── fb_pid.c/h             # PID 控制器
│   │   ├── fb_first_order.c/h     # 一阶惯性
│   │   ├── fb_ramp.c/h            # 斜坡函数
│   │   └── fb_limit.c/h           # 限幅功能块
│   └── python_bindings/           # Python C Extension
│       ├── plcopen_module.c       # Python 模块定义
│       ├── py_pid.c               # PID Python 包装
│       ├── py_first_order.c       # 一阶惯性 Python 包装
│       └── setup.py               # Python 扩展构建脚本
├── python/                        # Python 脚本和库
│   ├── plcopen/                   # Python 包（对 C 扩展的高层封装）
│   │   ├── __init__.py
│   │   ├── blocks.py              # 功能块 Python 类
│   │   └── utils.py               # 工具函数
│   ├── examples/                  # 示例脚本
│   │   └── pid_temperature.py    # PID 温度控制示例
│   └── debug_server.py            # debugpy 调试服务器启动脚本
├── tests/                         # 测试目录
│   ├── unit/                      # 单元测试
│   │   ├── test_fb_pid.c          # C 功能块单元测试
│   │   └── test_python_api.py    # Python API 单元测试
│   ├── integration/               # 集成测试
│   │   └── test_control_loop.py  # 控制回路集成测试
│   └── fixtures/                  # 测试数据
├── config/                        # 配置文件
│   ├── runtime.yaml               # 运行时配置示例
│   └── .env.example               # 环境变量模板
├── docs/                          # 用户文档
│   ├── api/                       # API 文档
│   └── user_guide.md              # 用户手册
├── scripts/                       # 构建和部署脚本
│   ├── build.sh                   # 构建脚本
│   └── setup_env.sh               # 环境初始化脚本
├── .gitattributes                 # Git 属性（强制 LF）
├── .gitignore                     # Git 忽略规则
├── Makefile                       # 构建系统
├── requirements.txt               # Python 依赖
├── README.md                      # 项目说明
└── LICENSE                        # 开源许可证（MIT 或 Apache 2.0）
```

**Structure Decision**: 采用单项目结构（Option 1）。C 语言核心功能块与 Python 绑定分离，便于独立测试和维护。使用 Makefile 统一构建 C 扩展和 Python 包。运行时主程序负责加载 Python 脚本和调度功能块，遵循关注点分离原则。

## Complexity Tracking

> 本节仅在 Constitution Check 存在违规且需要论证时填写。当前无违规项，留空。

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |

---

## Phase 0: Outline & Research

**Status**: ✅ 已完成 (2026-01-17)

### 研究任务概述

针对技术上下文中的未知项和依赖项，完成了以下研究：

1. **PLCopen 标准选择** → 决定采用 IEC 61131-3 标准
2. **Python 绑定方式** → 选择 Python C Extension API
3. **调度器实现** → 使用 C 主循环 + `clock_nanosleep()`
4. **调试协议** → 采用 debugpy（vs pdb）
5. **参数验证策略** → 限制+警告（Clamp and warn）
6. **日志系统** → 基于文件的日志系统

### 输出

**研究文档**: [research.md](research.md)

包含 6 个研究主题，每个主题包括：
- 决策（Decision）
- 理由（Rationale）
- 考虑的替代方案（Alternatives considered）

**关键结论**:
- IEC 61131-3 提供标准化的功能块接口和行为定义
- CPython C Extension API 提供最佳性能和嵌入式兼容性
- `clock_nanosleep()` 确保周期稳定性（CLOCK_MONOTONIC）
- debugpy 提供远程调试能力，与 VS Code 无缝集成
- 参数限制+警告策略防止控制系统不稳定，符合工业实践
- 文件日志系统简单可靠，避免第三方依赖

---

## Phase 1: Design & Contracts

**Status**: ✅ 已完成 (2026-01-17)

### 设计任务概述

基于研究文档，完成了以下设计工作：

1. **数据模型定义** → 5 个核心实体
2. **API 合约设计** → Python 功能块接口规范
3. **快速入门指南** → 用户上手教程

### 输出

#### 1. 数据模型 ([data-model.md](data-model.md))

**实体**:
- `FunctionBlock`（抽象基类）：功能块基础接口
- `PIDFunctionBlock`（继承 FunctionBlock）：PID 控制器，参数 Kp/Ki/Kd [0, 1e6]
- `FirstOrderFunctionBlock`（继承 FunctionBlock）：一阶惯性，参数 T [0.001, 1e6]
- `RuntimeContext`（单例）：运行时上下文，管理配置、解释器和功能块实例列表
- `DebugSession`：调试会话，状态机（IDLE → CONNECTING → CONNECTED → ERROR）

**关键设计决策**:
- 功能块参数超出范围自动限制（clamp）并发出警告
- PID 支持输出限幅（`output_min`/`output_max`）
- RuntimeContext 单例模式，生命周期与运行时一致
- DebugSession 状态机确保调试连接的可靠性

#### 2. API 合约 ([contracts/function_blocks_api.md](contracts/function_blocks_api.md))

**主要 API 类**:
- `plcopen.PID(Kp, Ki, Kd, output_min, output_max)`
  - `compute(SP, PV) -> float`
  - `set_params(Kp, Ki, Kd)`
  - `get_params() -> dict`
  - `reset()`
- `plcopen.FirstOrder(T)`
  - `compute(input) -> float`
  - `set_time_constant(T)`
  - `reset()`
- `plcopen.Ramp(rate)` - 斜坡生成器
- `plcopen.Limit(min, max)` - 信号限幅器

**用户脚本约定**:
- 必须实现 `init()` 函数（初始化）
- 必须实现 `step()` 函数（周期执行）
- 全局变量用于在 `init()` 和 `step()` 之间共享状态

**完整示例**: PID 温度控制（包含模拟物理模型）

#### 3. 快速入门指南 ([quickstart.md](quickstart.md))

**内容**:
- **第 1 步**: 安装运行时（源码构建 / 预编译包）
- **第 2 步**: 创建配置文件（`config.yaml`，定义周期时间、脚本路径、日志等）
- **第 3 步**: 编写控制脚本（`control_script.py`，PID 温度控制示例）
- **第 4 步**: 运行控制循环（`plcopen-runtime --config config.yaml`）
- **第 5 步**: 使用远程调试（VS Code + debugpy）
- **第 6 步**: 查看日志（`runtime.log`，周期抖动统计）
- **常见问题**: 安装失败、周期不稳定、远程调试连接、PID 振荡

**目标用户**: 嵌入式系统开发工程师，预计 30 分钟完成

### Agent 上下文更新

**文件**: `.github/agents/copilot-instructions.md`

已更新 GitHub Copilot 上下文文件，包含：
- 语言/版本：C11 + Python 3.8+
- 数据库配置：YAML/INI 配置文件
- 项目类型：单项目（嵌入式运行时）
- 最新功能：PLCopen Python 运行时环境

### Constitution Check（设计后重新评估）

✅ **再次通过所有宪章原则检查**

**关键确认**:
- 所有设计文档使用简体中文（API 文档、快速入门指南、数据模型）
- API 设计遵循 SOLID 原则（功能块接口隔离、单一职责）
- 依赖项保持最小化（仅 debugpy，无额外 C 库）
- 代码质量门禁在设计阶段通过评审

---

## Next Steps

执行 `/speckit.tasks` 命令生成 Phase 2 任务列表（`tasks.md`），包括：
- 任务分解（C 功能块实现、Python 绑定、运行时调度器等）
- 优先级排序（MVP 核心功能优先）
- 验收标准（单元测试、集成测试、性能基准）

---

**Generated by**: `/speckit.plan` command
**Branch**: `001-plcopen-python-runtime`
**Date**: 2026-01-17
