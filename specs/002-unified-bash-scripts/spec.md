# Feature Specification: 统一使用 Bash 脚本实现跨平台开发

**Feature Branch**: `002-unified-bash-scripts`
**Created**: 2026年1月18日
**Status**: Draft
**Input**: User description: "为了跨平台开发，有的人在windows上开发,有的在macos开发，有的在ubuntu开发，考虑统一使用bash脚本，而抛弃powershell"

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

- 当开发者在 Windows 上既没有 Git Bash 也没有 WSL 时，如何提供明确的错误提示和安装指引？
- 当脚本依赖的命令（如 `docker`、`make` 等）在某个平台上不可用时，如何优雅地处理错误？
- 当脚本需要访问不同平台的特定路径格式（Windows 的 `\` vs Unix 的 `/`）时，如何确保兼容性？
- 当在 Git Bash 中执行需要交互式输入的脚本时，如何确保终端行为正常？
- 当 Bash 版本过旧（如 macOS 默认的 Bash 3.x）时，如何处理兼容性问题？

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: 系统必须提供一套统一的 Bash 脚本，涵盖所有核心开发任务（构建、运行、测试、调试、Docker 管理）
- **FR-002**: 所有 Bash 脚本必须在 Windows（Git Bash/WSL）、macOS 和 Linux 上无修改地运行
- **FR-003**: 系统必须检测当前运行平台并自动处理平台特定的差异（路径格式、命令可用性等）
- **FR-004**: 所有现有的 PowerShell 脚本功能必须在 Bash 脚本中完整实现，不能有功能缺失
- **FR-005**: 脚本必须提供清晰的错误信息，当依赖的工具不可用时，给出明确的安装指引
- **FR-006**: 系统必须提供文档说明如何在每个平台上设置 Bash 环境（Git Bash 安装、WSL 配置等）
- **FR-007**: 脚本必须保持向后兼容，现有的工作流程不应因迁移而中断
- **FR-008**: 所有脚本必须包含适当的注释和帮助信息（通过 `--help` 参数查看）
- **FR-009**: 系统必须在项目根目录保留一个迁移说明文档，指导用户从 PowerShell 迁移到 Bash
- **FR-010**: 脚本必须支持常见的 Bash 版本（3.2+ 用于 macOS 兼容性，4.0+ 用于最佳体验）

### Key Entities

- **Bash 脚本集合**: 包含 build.sh、run.sh、debug.sh 等核心开发脚本，每个脚本对应原有的 .ps1 和 .bat 脚本功能
- **平台检测模块**: 可重用的 Bash 函数库，用于检测操作系统、路径格式转换、命令可用性检查
- **配置文件**: 环境变量配置和平台特定设置，支持用户自定义覆盖
- **迁移文档**: 记录 PowerShell 到 Bash 的映射关系、迁移步骤和常见问题解决方案

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 开发者能够在任意支持的平台（Windows/macOS/Linux）上使用相同的 Bash 脚本完成所有开发任务，无需修改脚本内容
- **SC-002**: 新开发者能够在 15 分钟内根据文档在其选择的平台上完成 Bash 环境设置并成功运行第一个脚本
- **SC-003**: 项目仓库中 PowerShell 脚本（.ps1 文件）的数量减少到 0，维护成本降低 50%（以脚本文件数量和代码行数计）
- **SC-004**: 90% 的开发者能够在不查看文档的情况下理解和使用新的 Bash 脚本（通过脚本内置帮助和清晰的命名实现）
- **SC-005**: 脚本执行时间在所有平台上的差异不超过 10%（确保跨平台兼容性代码不引入显著性能开销）
- **SC-006**: 在迁移后的一个月内，与脚本相关的支持请求或问题报告减少 40%
- **SC-007**: 所有核心脚本（构建、运行、测试）在 Bash 3.2+ 版本上正常工作，无语法错误或不兼容的特性

## Assumptions *(mandatory)*

- 假设 Windows 开发者愿意安装 Git Bash（随 Git for Windows 一起提供）或启用 WSL，这在现代 Windows 开发中是标准配置
- 假设所有开发者的系统上已经安装了项目所需的基础工具（Docker、Python、Make 等），脚本只需检测它们是否可用
- 假设 macOS 开发者使用的系统至少是 macOS 10.9+，包含 Bash 3.2 或更高版本
- 假设 Linux 开发者使用的发行版包含 Bash 4.0+ 和标准的 GNU 核心工具（coreutils）
- 假设项目现有的 PowerShell 脚本功能已经明确定义，可以通过代码审查和测试来确保 Bash 版本的功能等价性
- 假设团队同意在过渡期（1-2 周）内同时保留 PowerShell 和 Bash 脚本，逐步验证后再完全移除 PowerShell 脚本

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
- **现有工作流中断**: 如果迁移不充分，可能导致部分开发者的工作流程中断，需要提供过渡期和回退方案
- **性能问题**: 在 Windows 的 Git Bash 上，Bash 脚本执行可能比 PowerShell 慢，需要进行性能基准测试
- **维护者接受度**: 如果团队中有强烈依赖 PowerShell 的成员，可能面临抵触，需要提前沟通和培训

## Out of Scope

- 不考虑支持非 Bash shell（如 Zsh、Fish），团队统一使用 Bash
- 不考虑为每个脚本提供 GUI 界面或 IDE 集成，保持命令行工具的简洁性
- 不考虑将构建系统从 Make 迁移到其他工具（如 CMake），只关注脚本层面的统一
- 不考虑完全重写现有脚本逻辑，只做必要的语法转换和跨平台兼容性处理
- 不考虑支持 Windows 原生 CMD 或纯 PowerShell 环境（不安装 Git Bash/WSL），要求用户必须安装 Bash 环境

## Notes

- 建议在项目根目录添加 `.bashrc` 或 `.envrc` 文件来设置项目特定的环境变量，简化脚本配置
- 可以考虑使用 `shellcheck` 工具来验证 Bash 脚本质量，确保没有常见的语法错误或不良实践
- 在迁移过程中，可以保留 .bat 文件作为 Bash 脚本的简单包装器（例如 `build.bat` 调用 `bash build.sh`），方便 Windows 用户过渡
- 建议记录所有平台特定的 workaround 和兼容性处理代码，便于后续维护和问题排查

