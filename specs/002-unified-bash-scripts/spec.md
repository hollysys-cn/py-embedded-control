# Feature Specification: 统一使用 Bash 脚本实现跨平台开发

**Feature Branch**: `002-unified-bash-scripts`
**Created**: 2026年1月18日
**Status**: Draft
**Input**: User description: "为了跨平台开发，有的人在windows上开发,有的在macos开发，有的在ubuntu开发，考虑统一使用bash脚本，而抛弃powershell"

## Clarifications

### Session 2026-01-18

- Q: 在迁移过程中，如果开发者在 Windows 上既没有 Git Bash 也没有 WSL，脚本应该如何响应？ → A: 脚本立即退出并显示错误信息，包含 Git Bash/WSL 安装链接
- Q: 对于脚本的可观测性（日志记录、监控），在开发和生产环境中有什么具体要求？ → A: 使用结构化日志格式（时间戳、级别、消息），可重定向到文件
- Q: 在过渡期间，如果现有的 PowerShell 脚本和新的 Bash 脚本执行结果不一致时，应该如何处理？ → A: 始终优先使用 Bash 脚本，忽略差异继续推进
- Q: 对于脚本的环境检测和错误处理，应该采用什么策略？ → A: 不要过度检测环境，假设开发者已按文档准备好环境，尽量简化脚本和文档
- Q: 对于"去除多余的脚本"，以下哪些脚本应该保留？ → A: 仅保留：build.sh、run.sh、test.sh（核心工作流的3个脚本）

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Windows 开发者使用 Bash 脚本 (Priority: P1)

Windows 开发者希望使用统一的 Bash 脚本执行项目的构建、运行和调试任务，而不需要学习和维护 PowerShell 脚本。他们期望能够在 Git Bash、WSL 或其他 Bash 环境中无缝执行所有开发任务。

**Why this priority**: 这是最关键的用户场景，因为它解决了项目中脚本维护成本最高的问题。Windows 用户群体通常需要同时维护 .ps1 和 .sh 两套脚本，统一到 Bash 后可以大幅减少维护成本。

**Independent Test**: 可以通过在 Windows 的 Git Bash 或 WSL 环境中执行构建脚本（如 build.sh）并验证构建成功来独立测试。

**Acceptance Scenarios**:

1. **Given** Windows 开发者在 Git Bash 环境中，**When** 执行 `./build.sh`，**Then** 项目成功构建且输出与预期一致
2. **Given** Windows 开发者在 WSL 环境中，**When** 执行 `./run.sh`，**Then** 应用程序正常启动并运行
3. **Given** Windows 开发者需要调试，**When** 执行 `./debug.sh`，**Then** 调试环境正常启动且可以连接调试器

---

### User Story 2 - macOS 开发者使用统一脚本 (Priority: P2)

macOS 开发者希望使用与 Linux 和 Windows（通过 Bash）开发者相同的脚本，确保开发体验一致，并且不需要为 macOS 特定的差异创建额外的脚本变体。

**Why this priority**: macOS 是常见的开发平台，确保跨平台一致性对团队协作至关重要。但相比 Windows（优先级更高），macOS 原生支持 Bash，兼容性问题较少。

**Independent Test**: 可以在 macOS 系统上执行所有关键脚本（构建、运行、测试）并验证功能正常来独立测试。

**Acceptance Scenarios**:

1. **Given** macOS 开发者在终端中，**When** 执行 `./build.sh`，**Then** 项目成功构建
2. **Given** macOS 开发者在终端中，**When** 执行测试脚本，**Then** 所有单元测试通过
3. **Given** macOS 开发者查看脚本，**When** 对比 Windows/Linux 版本，**Then** 脚本内容完全一致（无平台特定分支）

---

### User Story 3 - Linux 开发者使用统一脚本 (Priority: P2)

Linux 开发者希望继续使用 Bash 脚本，但期望这些脚本经过优化和标准化，能够支持其他平台的开发者，并且不会因为跨平台兼容性修改而降低性能或易用性。

**Why this priority**: Linux 开发者已经在使用 Bash，但需要确保统一脚本不会因为适配其他平台而引入问题。这与 macOS 优先级相同，因为两者都是原生 Bash 环境。

**Independent Test**: 可以在 Ubuntu 或其他 Linux 发行版上执行脚本并验证功能完整性来独立测试。

**Acceptance Scenarios**:

1. **Given** Linux 开发者在终端中，**When** 执行 `./build.sh`，**Then** 构建速度和结果与之前相同或更好
2. **Given** Linux 开发者在终端中，**When** 执行 Docker 相关脚本，**Then** 容器管理功能正常工作
3. **Given** Linux 开发者查看脚本，**When** 检查跨平台兼容性代码，**Then** 兼容性处理简洁且不影响性能

---

### User Story 4 - 新团队成员快速上手 (Priority: P3)

新加入项目的开发者（无论使用何种操作系统）希望能够通过一套简单的 Bash 脚本快速完成开发环境设置，而不需要根据操作系统选择不同的脚本或学习多种脚本语言。

**Why this priority**: 这有助于降低新人入门门槛，但不如核心开发工作流程关键。

**Independent Test**: 可以让从未接触过项目的新人按照统一的文档在任意平台上完成环境设置来独立测试。

**Acceptance Scenarios**:

1. **Given** 新开发者克隆项目仓库，**When** 按照 README 执行设置脚本，**Then** 在 10 分钟内完成环境配置
2. **Given** 新开发者执行首次构建，**When** 运行 `./build.sh`，**Then** 构建成功且有清晰的输出信息
3. **Given** 新开发者遇到问题，**When** 查看脚本和文档，**Then** 能够理解脚本逻辑并自行排查问题

---

### Edge Cases

- 当脚本需要访问不同平台的特定路径格式（Windows 的 `\` vs Unix 的 `/`）时，如何确保兼容性？
- 当在 Git Bash 中执行需要交互式输入的脚本时，如何确保终端行为正常？
- 在过渡期，如果 Bash 脚本与 PowerShell 脚本结果不一致，团队将优先使用 Bash 版本并快速迭代修复问题

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: 系统必须提供一套精简的 Bash 脚本，仅包含核心开发任务：build.sh（构建）、run.sh（运行）、test.sh（测试），移除所有非必需脚本
- **FR-002**: 所有 Bash 脚本必须支持 Bash 3.2+(macOS 兼容)并在 Windows（Git Bash/WSL）、macOS 和 Linux 上无修改地运行
- **FR-003**: 系统必须检测当前运行平台并自动处理平台特定的差异（路径格式、命令可用性等）
- **FR-004**: 所有现有的 PowerShell 脚本功能必须在 Bash 脚本中完整实现，不能有功能缺失
- **FR-005**: 脚本应保持简洁，不进行过度的环境检测，假设开发者已按照文档要求准备好开发环境（Bash、Docker、Python 等）
- **FR-006**: 系统必须提供简洁的文档说明开发环境的前置条件，但不在脚本中重复检测这些条件
- **FR-007**: 脚本必须保持向后兼容，现有的工作流程不应因迁移而中断
- **FR-008**: 所有脚本必须包含简洁的注释和 `--help` 参数支持
- **FR-009**: 系统必须在项目根目录保留一个简洁的迁移说明文档
- **FR-010**: 脚本输出应保持简洁清晰，仅在必要时输出信息，避免过度冗长的日志

### Key Entities

- **Bash 脚本集合**: 仅包含 3 个核心脚本：build.sh（构建项目）、run.sh（运行应用）、test.sh（执行测试），移除调试脚本和 Docker 管理脚本（可通过直接命令或 tasks.json 实现）
- **平台检测模块**: 轻量级的平台检测逻辑（如果需要），嵌入到各脚本中而非独立模块
- **配置文件**: 环境变量配置和平台特定设置，支持用户自定义覆盖
- **迁移文档**: 记录 PowerShell 到 Bash 的映射关系、迁移步骤和常见问题解决方案

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 开发者能够在任意支持的平台（Windows/macOS/Linux）上使用相同的 Bash 脚本完成所有开发任务，无需修改脚本内容
- **SC-002**: 新开发者能够在 15 分钟内根据文档在其选择的平台上完成 Bash 环境设置并成功运行第一个脚本
- **SC-003**: 项目仓库中 PowerShell 脚本（.ps1 文件）的数量减少到 0，脚本总数从当前的 10+ 个减少到仅 3 个核心脚本，维护成本降低 70% 以上
- **SC-004**: 90% 的开发者能够在不查看文档的情况下理解和使用新的 Bash 脚本（通过脚本内置帮助和清晰的命名实现）
- **SC-005**: 脚本执行时间在所有平台上的差异不超过 10%（确保跨平台兼容性代码不引入显著性能开销）
- **SC-006**: 在迁移后的一个月内，与脚本相关的支持请求或问题报告减少 40%
- **SC-007**: 所有核心脚本（构建、运行、测试）在 Bash 3.2+ 版本上正常工作，无语法错误或不兼容的特性
- **SC-008**: 脚本代码行数比 PowerShell 版本减少至少 30%，文档篇幅减少至少 40%，体现简化原则

## Assumptions *(mandatory)*

- 假设 Windows 开发者已经安装 Git Bash（随 Git for Windows 一起提供）或启用 WSL
- 假设所有开发者的系统上已经按照文档要求安装了项目所需的基础工具（Docker、Python、Make 等）
- 假设开发者在执行脚本之前会阅读文档，了解前置条件和环境要求
- 假设 macOS 开发者使用的系统至少是 macOS 10.9+，包含 Bash 3.2 或更高版本
- 假设 Linux 开发者使用的发行版包含 Bash 4.0+ 和标准的 GNU 核心工具（coreutils）
- 假设项目现有的 PowerShell 脚本功能已经明确定义，可以通过代码审查和测试来确保 Bash 版本的功能等价性
- 假设团队同意采用激进的迁移策略：在过渡期间优先使用 Bash 脚本，如遇到与 PowerShell 的行为差异，将快速迭代修复 Bash 版本而非回退
- 假设过渡期为 1-2 周，期间会保留 PowerShell 脚本作为参考，但鼓励开发者优先测试和使用 Bash 版本

## Dependencies *(include if applicable)*

- Git for Windows (包含 Git Bash) 或 WSL (Windows Subsystem for Linux) - Windows 平台必需
- Bash 3.2+ - macOS 默认提供
- Bash 4.0+ - Linux 通常默认提供，用于更好的特性支持
- Docker - 如果脚本需要管理容器
- 项目现有依赖（Python、Make 等）- 与平台无关，但脚本需要检测它们

## Risks *(include if applicable)*

- **学习曲线**: Windows 开发者可能不熟悉 Bash 语法和 Unix 工具，需要提供清晰的文档和示例
- **Git Bash 限制**: Git Bash 在 Windows 上可能有一些限制（如伪终端支持、路径转换），需要仔细测试
- **Bash 版本差异**: macOS 默认使用较旧的 Bash 3.2（由于许可证原因），需要避免使用 Bash 4.0+ 的新特性，或提供替代方案
- **现有工作流中断**: 由于采用激进的迁移策略（优先使用 Bash 脚本），在过渡期可能会出现功能差异导致的工作流中断，需要团队快速响应和修复
- **性能问题**: 在 Windows 的 Git Bash 上，Bash 脚本执行可能比 PowerShell 慢，需要进行性能基准测试
- **维护者接受度**: 如果团队中有强烈依赖 PowerShell 的成员，可能面临抵触，需要提前沟通和培训

## Out of Scope

- 不考虑支持非 Bash shell（如 Zsh、Fish），团队统一使用 Bash
- 不考虑为每个脚本提供 GUI 界面或 IDE 集成，保持命令行工具的简洁性
- 不考虑将构建系统从 Make 迁移到其他工具（如 CMake），只关注脚本层面的统一
- 不考虑完全重写现有脚本逻辑，只做必要的语法转换和跨平台兼容性处理
- 不考虑支持 Windows 原生 CMD 或纯 PowerShell 环境（不安装 Git Bash/WSL），要求用户必须安装 Bash 环境
- 不提供独立的调试脚本（debug.sh），开发者可直接使用 IDE 调试功能或手动启动调试器
- 不提供独立的 Docker 管理脚本，开发者可直接使用 docker-compose 命令或 VSCode tasks

## Notes

- 建议在项目根目录添加 `.bashrc` 或 `.envrc` 文件来设置项目特定的环境变量，简化脚本配置
- 可以考虑使用 `shellcheck` 工具来验证 Bash 脚本质量，确保没有常见的语法错误或不良实践
- 建议记录所有平台特定的 workaround 和兼容性处理代码，便于后续维护和问题排查

