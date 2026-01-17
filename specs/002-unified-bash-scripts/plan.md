# Implementation Plan: 统一使用 Bash 脚本实现跨平台开发

**Branch**: `002-unified-bash-scripts` | **Date**: 2026年1月18日 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/002-unified-bash-scripts/spec.md`

## Summary

将项目中的 PowerShell 脚本（.ps1）迁移到 Bash 脚本（.sh），实现 Windows（Git Bash/WSL）、macOS 和 Linux 三平台的统一开发体验。采用极简设计哲学，仅保留 3 个核心脚本（build.sh、run.sh、test.sh），移除所有非必需脚本和过度的环境检测，假设开发者已按文档准备好环境。脚本将支持 Docker 容器管理、项目构建和测试运行，同时保持简洁清晰的输出。

## Technical Context

**Language/Version**: Bash 3.2+（macOS 兼容）/ 4.0+（最佳体验）
**Primary Dependencies**:
- Git Bash 或 WSL（Windows 平台）
- Docker & docker-compose（容器管理）
- Make（构建系统）
- Python 3.11+（运行时）

**Storage**: N/A（脚本操作，无持久化需求）
**Testing**: 手动测试 + 跨平台验证（Windows/macOS/Linux）
**Target Platform**: 跨平台（Windows 10/11 + Git Bash/WSL, macOS 10.9+, Ubuntu 20.04+）
**Project Type**: 单一项目（嵌入式控制系统）
**Performance Goals**: 脚本执行时间在各平台差异 <10%
**Constraints**:
- 支持 Bash 3.2+（macOS 默认版本）
- 脚本代码行数比 PowerShell 版本减少 30%+
- 不进行过度的环境检测和依赖检查
**Scale/Scope**:
- 3 个核心脚本（build.sh、run.sh、test.sh）
- 移除 10+ 个现有脚本（.ps1, .bat, debug/docker 专用脚本）

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

基于 `.specify/memory/constitution.md` 的核心原则进行检查：

- [x] **简体中文优先**：所有文档、注释、界面文本使用简体中文 ✓
- [x] **SOLID 原则**：脚本保持单一职责（构建/运行/测试各司其职），模块化清晰 ✓
- [x] **架构兼容性**：不修改现有源代码目录结构，仅替换根目录脚本文件 ✓
- [x] **环境隔离**：脚本不污染全局环境，依赖项目内 Docker 和虚拟环境 ✓
- [x] **编程语言标准**：Bash 脚本遵循 POSIX 兼容性，支持 Bash 3.2+ ✓
- [x] **依赖管理**：无新增第三方依赖，使用现有工具（Docker, Make, Git Bash） ✓
- [x] **许可合规**：Bash 脚本为项目自有代码，无第三方许可问题 ✓
- [x] **代码质量**：脚本包含简体中文注释，逻辑简洁，可使用 shellcheck 静态分析 ✓
- [x] **修改范围**：最小化修改，仅替换脚本文件，不触及 src/、python/、tests/ 等源码目录 ✓
- [x] **文件编码**：所有脚本使用 UTF-8 + LF 行尾符 ✓
- [x] **质量门禁**：通过 shellcheck 静态分析，手动跨平台测试验证 ✓

**评估结果**: ✅ 通过 - 无宪章违反项

## Project Structure

### Documentation (this feature)

```text
specs/002-unified-bash-scripts/
├── plan.md              # 本文件（实施计划）
├── spec.md              # 功能规范
├── research.md          # Phase 0 输出（研究和决策）
├── data-model.md        # Phase 1 输出（数据模型）
├── quickstart.md        # Phase 1 输出（快速开始指南）
└── contracts/           # Phase 1 输出（脚本接口契约）
    ├── build.sh.md      # build.sh 接口规范
    ├── run.sh.md        # run.sh 接口规范
    └── test.sh.md       # test.sh 接口规范
```

### Source Code (repository root)

```text
py-embedded-control/
├── build.sh                    # ✨ 新增：构建脚本（替代 build.ps1）
├── run.sh                      # ✨ 新增：运行脚本（替代 run.ps1）
├── test.sh                     # ✨ 新增：测试脚本（整合多个测试脚本）
├── MIGRATION_FROM_POWERSHELL.md # ✨ 新增：迁移指南文档
├── build.ps1                   # 🗑️ 过渡期保留，最终移除
├── build.bat                   # 🗑️ 过渡期保留，最终移除
├── run.ps1                     # 🗑️ 过渡期保留，最终移除
├── run.bat                     # 🗑️ 过渡期保留，最终移除
├── debug*.ps1                  # 🗑️ 移除（使用 VSCode 调试）
├── start-debug.*               # 🗑️ 移除（使用 VSCode 调试）
├── scripts/
│   ├── debug-container.*       # 🗑️ 移除（直接使用 docker-compose）
│   └── add_copyright.ps1       # ⚠️ 保留（非开发脚本，版权工具）
├── tests/
│   ├── test_*.sh               # ✏️ 保留但由 test.sh 统一调用
│   └── ...
├── README.md                   # ✏️ 更新：快速开始部分
├── docs/
│   └── guides/
│       ├── WINDOWS_QUICKSTART.md  # ✏️ 更新：Git Bash/WSL 安装指引
│       └── DEBUGGING.md           # ✏️ 更新：不再使用 debug.sh
└── PROJECT_FILES.md            # ✏️ 更新：脚本文件清单

图例:
✨ 新增文件
🗑️ 移除文件
✏️ 更新文件
⚠️ 保留不变
```

### 文件操作计划

**Phase 2（实施阶段）将执行**:

1. **新增**: build.sh, run.sh, test.sh
2. **新增**: MIGRATION_FROM_POWERSHELL.md
3. **更新**: README.md, docs/guides/WINDOWS_QUICKSTART.md, PROJECT_FILES.md
4. **过渡期保留**: build.ps1, run.ps1, *.bat（1-2 周后删除）
5. **立即删除**: debug*.ps1, start-debug.*, scripts/debug-container.*

ios/ or android/
└── [platform-specific structure: feature modules, UI flows, platform tests]
```

**Structure Decision**: [Document the selected structure and reference the real
directories captured above]

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
