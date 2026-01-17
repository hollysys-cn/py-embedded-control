# Tasks: PLCopen Python 运行时环境

**Branch**: `001-plcopen-python-runtime`
**Generated**: 2026-01-17
**Input**: [plan.md](plan.md), [spec.md](spec.md), [data-model.md](data-model.md), [contracts/function_blocks_api.md](contracts/function_blocks_api.md), [research.md](research.md)

**注意**: 本文档基于用户故事组织任务，每个故事可独立实现和测试。测试任务已省略（规格说明中未明确要求 TDD）。

## 任务格式：`- [ ] [TaskID] [P?] [Story?] Description with file path`

- **[P]**: 可并行执行（不同文件，无依赖）
- **[Story]**: 所属用户故事（US1、US2、US3）
- **文件路径**: 每个任务包含精确的文件路径

---

## Phase 1: Setup（项目初始化）

**目的**: 建立项目结构和开发环境

- [X] T001 根据 plan.md 创建项目目录结构（src/, python/, tests/, config/, docs/）
- [X] T002 创建 .gitignore 和 .gitattributes（强制 UTF-8 无 BOM，LF 行尾）
- [X] T003 [P] 创建 Makefile 定义构建目标（build, clean, install, test, docker-build, docker-dev, docker-arm）
- [X] T004 [P] 创建 requirements.txt 定义 Python 依赖（debugpy, PyYAML, pytest）
- [X] T005 [P] 创建 README.md 包含项目说明、快速开始（Docker 和原生方式）、许可证信息
- [X] T006 [P] 创建配置文件模板 config/runtime.yaml.example（周期时间、脚本路径、日志配置）
- [X] T007 创建 Python 包结构 python/plcopen/__init__.py（版本号、包导出）
- [X] T007a [P] 创建 Dockerfile（多架构支持 x86_64/ARM，基于 debian:stable，安装 GCC/Python3/cppcheck，配置阿里云 PyPI 镜像）
- [X] T007b [P] 创建 docker-compose.yml（定义 dev 服务 x86_64 和 arm-test 服务 ARM64，映射调试端口 5678/5679，挂载工作目录）
- [X] T007c [P] 创建 .dockerignore（排除 .git、__pycache__、*.pyc、*.o、*.so、build/、docs/、.vscode/）

**检查点**: 项目结构就绪，Docker 开发环境可用（支持 x86_64 开发和 ARM 功能测试），可以开始开发基础设施代码

---

## Phase 2: Foundational（阻塞性基础设施）

**目的**: 所有用户故事依赖的核心组件，必须在用户故事实现前完成

**⚠️ 关键**: 此阶段完成前，用户故事实现无法开始

### 日志系统

- [X] T008 实现日志系统 src/runtime/logger.c/h（支持 DEBUG/INFO/WARNING/ERROR 级别）
- [X] T009 实现日志轮转机制（基于文件大小，支持配置最大尺寸）

### 配置管理

- [X] T010 实现配置加载器 src/runtime/config_loader.c/h（解析 YAML 配置文件）
- [X] T011 实现配置结构体定义 src/runtime/config.h（RuntimeConfig，包含周期时间、脚本路径）

### Python 嵌入

- [X] T012 实现 Python 解释器初始化 src/runtime/py_embed.c/h（Py_Initialize, 设置模块路径）
- [X] T013 实现 Python 脚本加载和执行 src/runtime/py_embed.c（导入用户脚本，调用 init/step 函数）
- [X] T014 实现 Python 异常处理 src/runtime/py_embed.c（捕获 PyErr, 记录日志，防止崩溃）

### 调度器

- [X] T015 实现周期调度器 src/runtime/scheduler.c/h（使用 clock_nanosleep(CLOCK_MONOTONIC)）
- [X] T016 实现周期抖动监控 src/runtime/scheduler.c（记录实际周期时间，超过阈值发出警告）

**检查点**: 基础设施完成，用户故事可以并行实现

---

## Phase 3: 用户故事 1 - 基础功能块调用（优先级: P1）🎯 MVP

**目标**: 在 Python 脚本中调用 PLCopen 功能块（PID、一阶惯性），快速实现控制逻辑

**独立测试**: 编写简单 Python 脚本调用 PID 功能块，输入设定值和反馈值，验证输出符合 PID 算法

### C 功能块实现

- [X] T017 [P] 定义功能块通用接口 src/function_blocks/fb_common.h（FunctionBlock 结构体，状态、参数、计算方法）
- [X] T018 [P] 实现 PID 控制器 src/function_blocks/fb_pid.c/h（位置式 PID，支持 Kp/Ki/Kd 参数）
- [X] T019 [P] 实现 PID 参数验证 src/function_blocks/fb_pid.c（限制 Kp/Ki/Kd 到 [0, 1e6]，超出范围 clamp + 警告）
- [X] T020 [P] 实现 PID 输出限幅 src/function_blocks/fb_pid.c（output_min/output_max 限制）
- [X] T021 [P] 实现 PID 状态管理 src/function_blocks/fb_pid.c（积分累积、上一周期误差存储）
- [X] T022 [P] 实现一阶惯性功能块 src/function_blocks/fb_first_order.c/h（传递函数 H(s)=1/(Ts+1)）
- [X] T023 [P] 实现一阶惯性参数验证 src/function_blocks/fb_first_order.c（时间常数 T [0.001, 1e6]）

### Python C Extension 绑定

- [X] T024 创建 Python 模块定义 src/python_bindings/plcopen_module.c（PyModuleDef, 模块初始化函数）
- [X] T025 [P] 实现 PID Python 包装类 src/python_bindings/py_pid.c（PIDObject, __init__, compute, set_params, get_params, reset 方法）
- [X] T026 [P] 实现 PID Python 类型定义 src/python_bindings/py_pid.c（PyTypeObject, 方法表，文档字符串）
- [X] T027 [P] 实现一阶惯性 Python 包装类 src/python_bindings/py_first_order.c（FirstOrderObject, compute, set_time_constant）
- [X] T028 实现 Python 参数解析 src/python_bindings/py_pid.c（PyArg_ParseTuple, 处理浮点数参数）
- [X] T029 实现 Python 异常转换 src/python_bindings/py_pid.c（C 参数验证失败 → Python ValueError/RuntimeError）
- [X] T030 创建 setup.py 构建脚本 src/python_bindings/setup.py（setuptools Extension, 指定 C 源文件和编译选项）

### Python 高层封装

- [X] T031 [P] 实现 Python 功能块类 python/plcopen/blocks.py（PID, FirstOrder 类，封装 C 扩展接口）
- [X] T032 [P] 添加 Python 类型提示 python/plcopen/blocks.py（使用 typing 模块，符合 PEP 484）
- [X] T033 [P] 实现参数警告机制 python/plcopen/blocks.py（参数超出范围时使用 warnings.warn()）

### 文档

- [X] T034 [P] 创建 API 使用示例 python/examples/basic_pid.py（创建 PID 实例，调用 compute 方法）
- [X] T035 [P] 添加 API 文档注释 python/plcopen/blocks.py（docstring，中文说明参数、返回值、异常）

**检查点**: 用户故事 1 完成，可以在 Python 中调用 PID 和一阶惯性功能块

---

## Phase 4: 用户故事 2 - 完整控制回路示例（优先级: P2）

**目标**: 运行完整的 PID 温度控制示例程序，验证闭环控制场景

**独立测试**: 运行温度控制示例（模拟传感器和执行器），验证 PID 控制器使模拟温度收敛到设定值

### 运行时主程序

- [X] T036 实现主程序入口 src/runtime/main.c（解析命令行参数 --config）
- [X] T037 实现运行时初始化 src/runtime/main.c（加载配置、初始化日志、启动 Python 解释器）
- [X] T038 实现控制循环主逻辑 src/runtime/main.c（调度器启动，周期性调用 Python step() 函数）
- [X] T039 实现优雅退出处理 src/runtime/main.c（捕获 SIGINT/SIGTERM，清理资源，关闭 Python 解释器）
- [X] T040 实现运行时上下文管理 src/runtime/context.c/h（RuntimeContext 单例，管理配置、解释器、功能块列表）

### 功能块扩展

- [X] T041 [P] 实现斜坡功能块 src/function_blocks/fb_ramp.c/h（以固定速率变化输出）
- [X] T042 [P] 实现限幅功能块 src/function_blocks/fb_limit.c/h（限制输入到 [min, max] 范围）
- [X] T043 [P] 实现斜坡和限幅 Python 绑定 src/python_bindings/py_ramp.c, py_limit.c

### 示例程序

- [X] T044 创建 PID 温度控制示例 python/examples/pid_temperature.py（包含 init() 和 step() 函数）
- [X] T045 实现温度物理模型模拟 python/examples/pid_temperature.py（加热效果、自然散热、测量噪声）
- [X] T046 添加示例配置文件 config/pid_temperature.yaml（周期时间 100ms，脚本路径指向示例）
- [X] T047 创建示例说明文档 docs/examples/pid_temperature.md（中文说明示例目的、参数调整、预期结果）

### 脚本约定实现

- [X] T048 实现用户脚本 init() 函数调用 src/runtime/py_embed.c（运行时启动时调用一次）
- [X] T049 实现用户脚本 step() 函数周期调用 src/runtime/py_embed.c（每个控制周期调用）
- [X] T050 实现脚本超时检测 src/runtime/scheduler.c（step() 执行时间超过周期时间记录警告）

**检查点**: 用户故事 2 完成，可以运行完整的 PID 温度控制示例 ✅

---

## Phase 5: 用户故事 3 - 远程调试 Python 脚本（优先级: P3）

**目标**: 从开发机远程连接到 Linux 设备，调试正在运行的 Python 控制脚本

**独立测试**: 在开发机启动调试客户端（VS Code），连接到设备，设置断点、单步执行、查看变量

### 调试服务器

- [X] T051 实现 debugpy 服务器启动 src/runtime/debug_server.c/h（调用 Python debugpy.listen()）
- [X] T052 实现调试配置解析 src/runtime/config_loader.c（从配置文件读取 debug.enabled, debug.host, debug.port）
- [X] T053 实现调试会话状态管理 src/runtime/debug_session.c/h（DebugSession 结构体，状态机：IDLE → CONNECTING → CONNECTED → ERROR）
- [X] T054 实现调试连接超时处理 src/runtime/debug_server.c（连接超时 5 秒，记录警告）
- [X] T055 实现调试独立性保证 src/runtime/debug_server.c（调试会话中断时控制逻辑继续运行）

### Python 调试支持

- [X] T056 [P] 创建调试服务器启动脚本 python/debug_server.py（import debugpy, debugpy.listen(), 等待连接）
- [X] T057 [P] 添加调试配置到示例 config/pid_temperature_debug.yaml（启用调试，设置主机和端口）

### 文档

- [X] T058 [P] 创建调试配置指南 docs/debugging.md（VS Code launch.json 配置，连接步骤，常见问题）
- [X] T059 [P] 添加调试示例截图 docs/debugging.md（断点设置、变量查看、单步执行）

**检查点**: 用户故事 3 完成，可以远程调试 Python 控制脚本

---

## Phase 6: Polish & Cross-Cutting Concerns（打磨和通用关注点）

**目的**: 提升代码质量、性能和用户体验

### 错误处理和健壮性

- [X] T060 [P] 实现所有 C 函数的空指针检查（NULL pointer guard）
- [X] T061 [P] 实现内存泄漏检测 tests/memory_leak_test.sh（运行 24 小时，监控内存增长）
- [X] T062 [P] 实现 Python 脚本加载失败处理 src/runtime/py_embed.c（脚本不存在或语法错误时优雅退出）

### 性能优化

- [X] T063 [P] 优化 PID 计算性能 src/function_blocks/fb_pid.c（避免重复计算，内联关键函数）
- [X] T064 [P] 实现 CPU 亲和性设置 src/runtime/scheduler.c（根据配置绑定到指定 CPU 核心）
- [X] T065 [P] 实现周期稳定性基准测试 tests/benchmark/cycle_stability.py（测量 95% 周期在 ±5% 范围内）

### 文档完善

- [X] T066 [P] 创建用户手册 docs/user_guide.md（安装、配置、运行、调试，中文）
- [X] T067 [P] 创建 API 参考文档 docs/api_reference.md（所有功能块方法、参数、返回值，中文）
- [X] T068 [P] 创建常见问题解答 docs/faq.md（安装失败、周期不稳定、调试连接、PID 振荡）
- [X] T069 [P] 创建贡献指南 docs/CONTRIBUTING.md（代码规范、提交流程、测试要求）

### 代码质量

- [X] T070 [P] 运行 cppcheck 静态分析 Makefile（添加 make lint 目标，检查 C 代码）
- [X] T071 [P] 运行 pylint/flake8 分析 Makefile（检查 Python 代码，PEP 8 规范）
- [X] T072 [P] 确保所有 C/Python 注释使用简体中文（符合 constitution.md 原则一）
- [X] T073 [P] 验证所有文件使用 UTF-8 无 BOM，LF 行尾（通过 .gitattributes 强制）

### 许可证和合规

- [X] T074 [P] 创建 LICENSE 文件（选择 MIT 或 Apache 2.0）
- [X] T075 [P] 添加依赖许可证说明 docs/licenses.md（debugpy MIT, CPython PSF License）
- [X] T076 [P] 添加版权声明到所有源文件头部（C 和 Python 文件）

---

## 依赖关系图（用户故事完成顺序）

```text
Phase 1 (Setup)
    ↓
Phase 2 (Foundational) ← 阻塞性依赖，必须先完成
    ↓
    ├─→ Phase 3 (US1: 基础功能块) ← MVP，最高优先级
    │       ↓
    ├─→ Phase 4 (US2: 完整控制回路) ← 依赖 US1 的功能块
    │       ↓ (可选依赖)
    └─→ Phase 5 (US3: 远程调试) ← 独立于 US1/US2，可并行实现
            ↓
        Phase 6 (Polish) ← 所有功能完成后
```

**关键依赖**:
- Phase 2 必须在所有用户故事前完成（阻塞性）
- US1 是 MVP，必须最先实现
- US2 依赖 US1 的功能块（PID、一阶惯性）
- US3 独立于 US1/US2，可并行实现（但通常在 US2 后更有价值）

---

## 并行执行机会（每个故事内）

### 用户故事 1 内的并行任务

**批次 1（C 功能块实现，无依赖）**:
- T018 (PID 实现) || T022 (一阶惯性实现) || T017 (通用接口定义)

**批次 2（Python 绑定，依赖批次 1）**:
- T025 (PID Python 包装) || T027 (一阶惯性 Python 包装) || T024 (模块定义)

**批次 3（高层封装和文档，依赖批次 2）**:
- T031 (Python 类) || T034 (示例代码) || T035 (文档注释)

### 用户故事 2 内的并行任务

**批次 1（运行时核心，可并行）**:
- T036 (主程序入口) || T040 (运行时上下文) || T041 (斜坡功能块) || T042 (限幅功能块)

**批次 2（示例和文档）**:
- T044 (示例程序) || T046 (配置文件) || T047 (示例文档)

### 用户故事 3 内的并行任务

**批次 1（调试服务器和文档）**:
- T051 (debugpy 服务器) || T056 (调试启动脚本) || T058 (调试文档)

---

## 实施策略

### MVP 优先（最小可行产品）

**第 1 步**: 完成 Phase 1 + Phase 2（基础设施）
**第 2 步**: 完成 Phase 3（用户故事 1 - 基础功能块）
**第 3 步**: 交付 MVP 给早期用户测试

此时系统已具备核心价值：在 Python 中调用 PLCopen 功能块。

### 增量交付

**第 4 步**: 完成 Phase 4（用户故事 2 - 完整控制回路示例）
**第 5 步**: 交付完整示例，用户可直接运行 PID 温度控制

**第 6 步**: 完成 Phase 5（用户故事 3 - 远程调试）
**第 7 步**: 交付调试功能，提升开发效率

**第 8 步**: 完成 Phase 6（打磨和优化）
**第 9 步**: 正式发布 1.0.0 版本

### 质量门禁

每个 Phase 完成后必须通过：
- **代码审查**: 符合 constitution.md 所有原则
- **静态分析**: cppcheck (C) 和 pylint/flake8 (Python) 无严重错误
- **功能测试**: 独立测试标准通过（每个用户故事）
- **性能测试**: 周期稳定性、CPU 占用、内存占用符合 success criteria

---

## 验收标准映射（任务 → 成功标准）

| 成功标准 | 相关任务 | 验证方法 |
|---------|---------|---------|
| SC-001: 10 分钟上手 | T044-T047, T066 | 用户测试，计时从阅读文档到运行示例 |
| SC-002: 95% 周期稳定性 | T015-T016, T065 | 基准测试，记录 1000 个周期的实际时间 |
| SC-003: 10 个功能块 <50% CPU | T063, T064 | 性能测试，ARM Cortex-A7 上运行 10 个 PID 实例 |
| SC-004: 调试连接 <5s | T051, T054 | 集成测试，测量 debugpy 连接建立时间 |
| SC-005: PID 精度 <0.1% 误差 | T018-T021 | 单元测试，与标准参考实现对比输出 |
| SC-006: 24h 内存增长 <10% | T061 | 长时间运行测试，每小时记录内存占用 |
| SC-007: 90% 用户调试成功 | T058-T059 | 用户测试，首次尝试配置调试的成功率 |
| SC-008: 示例代码 <50 行 | T034, T044 | 代码审查，检查示例代码行数 |

---

## 总计

- **总任务数**: 76
- **用户故事 1 任务数**: 19（T017-T035）
- **用户故事 2 任务数**: 15（T036-T050）
- **用户故事 3 任务数**: 9（T051-T059）
- **Setup + Foundational 任务数**: 16（T001-T016）
- **Polish 任务数**: 17（T060-T076）

**并行机会**: 约 40% 的任务标记为 [P]，可在同一阶段内并行执行

**MVP 范围**: Phase 1 + Phase 2 + Phase 3 = 35 任务（约 46% 的工作量）

---

**下一步**: 开始执行 Phase 1 任务，创建项目结构和开发环境

---

**Generated by**: `/speckit.tasks` command
**Branch**: `001-plcopen-python-runtime`
**Date**: 2026-01-17
