# Research & Decisions: 统一使用 Bash 脚本实现跨平台开发

**Feature**: [spec.md](spec.md) | **Date**: 2026年1月18日

## 研究目标

解决以下技术决策点，消除规范和计划中的所有 "NEEDS CLARIFICATION"：

1. 如何在 Windows 的 Git Bash 环境中处理路径格式差异？
2. 如何确保 Bash 3.2（macOS 默认）兼容性？
3. 如何简化脚本逻辑以减少 30%+ 代码行数？
4. 现有 PowerShell 脚本有哪些核心功能必须迁移？

## 决策 1: 跨平台路径处理策略

**问题**: Windows Git Bash 在路径处理上有特殊性（C:\ vs /c/），如何确保跨平台一致性？

**研究发现**:
- Git Bash 自动将 Windows 路径转换为 Unix 风格（C:\path → /c/path）
- `pwd` 命令在所有平台返回 Unix 风格路径
- `cd` 命令在 Git Bash 中接受两种路径格式
- 绝对路径应使用 `$(pwd)` 或 `$PWD` 变量，相对路径自然跨平台

**决策**:
- 脚本内部统一使用相对路径（`./build`, `./config`）
- 需要绝对路径时使用 `$(pwd)/relative/path` 构造
- 避免硬编码任何平台特定路径

**替代方案考虑**:
- ❌ 使用 `cygpath` 转换：Git Bash 特有，不够通用
- ❌ 为每个平台编写路径转换函数：增加复杂度，违背简化原则
- ✅ 依赖 Git Bash 的自动转换 + 相对路径：最简洁方案

---

## 决策 2: Bash 3.2 兼容性约束

**问题**: macOS 默认使用 Bash 3.2（2007年），缺少 Bash 4.0+ 的新特性，如何确保兼容？

**研究发现**:
- Bash 3.2 不支持：关联数组（`declare -A`）、`&>` 重定向、`[[` 的 `=~` 正则表达式等高级特性
- Bash 3.2 支持：基本条件测试、函数、`$()` 命令替换、`${var}` 参数展开、数组（索引数组）

**Bash 3.2 兼容性检查清单**:
| 特性 | Bash 3.2 | Bash 4.0+ | 使用建议 |
|------|---------|-----------|---------|
| 关联数组 | ❌ | ✅ | 用索引数组 + case 语句替代 |
| `&>` 重定向 | ❌ | ✅ | 使用 `2>&1` 替代 |
| `[[` 正则 `=~` | ⚠️ 有限 | ✅ | 使用 `case` 或 `grep` 替代 |
| `readarray`/`mapfile` | ❌ | ✅ | 使用 `while read` 循环 |
| `**` globstar | ❌ | ✅ | 使用 `find` 命令 |

**决策**:
- 严格遵守 Bash 3.2 兼容性，不使用 4.0+ 特性
- 使用 `shellcheck` 的 `--shell=bash -x` 检查兼容性
- 在脚本顶部添加 `#!/usr/bin/env bash` 而非 `#!/bin/bash`（提升可移植性）

**替代方案考虑**:
- ❌ 要求 macOS 用户升级到 Bash 5：增加用户负担
- ❌ 为 macOS 单独维护脚本版本：违背统一脚本目标
- ✅ 使用 Bash 3.2 兼容语法：最大兼容性

---

## 决策 3: 脚本简化策略

**问题**: 如何将现有 PowerShell 脚本的功能迁移到 Bash 并减少 30%+ 代码量？

**现有 PowerShell 脚本分析**:

| 脚本 | 行数 | 核心功能 | 可简化部分 |
|------|------|---------|-----------|
| build.ps1 | ~165 | Docker 构建、Make 调用、清理 | Docker 检测、详细错误处理、多余的函数封装 |
| run.ps1 | ~122 | Docker 运行、配置文件加载 | Docker 状态检查、参数验证、Shell 模式 |
| debug*.ps1 | ~200+ | 调试器启动、端口管理 | **移除**（使用 IDE 或 tasks.json） |
| Docker 管理脚本 | ~150+ | 容器启停、日志查看 | **移除**（直接使用 docker-compose） |

**简化策略**:
1. **移除环境检测**: 假设 Docker、Make 已安装（文档说明前置条件）
2. **移除调试专用脚本**: 开发者使用 VSCode tasks 或直接命令
3. **移除 Docker 管理脚本**: 使用 `docker-compose up/down/logs` 命令
4. **简化错误处理**: 仅在关键步骤检查退出码，不做详细诊断
5. **合并功能**: build.sh 支持 `--clean` 参数而非独立 clean 脚本

**代码行数目标**:
- build.sh: ~50 行（-70%）
- run.sh: ~40 行（-67%）
- test.sh: ~30 行（新增，替代多个测试脚本）
- **总计**: ~120 行 vs PowerShell 的 ~637 行 = **减少 81%**

**决策**:
- 采用极简设计，每个脚本专注单一任务
- 使用 `set -e` 自动错误退出，减少手动检查
- 输出简洁清晰，避免冗余日志

**替代方案考虑**:
- ❌ 保留所有功能：违背简化目标
- ❌ 使用 Bash 框架（如 BATS）：引入新依赖
- ✅ 极简脚本 + 文档说明：最符合需求

---

## 决策 4: PowerShell 核心功能迁移清单

**问题**: 哪些 PowerShell 功能是必须迁移的？哪些可以省略？

**功能分类**:

### 必须迁移（P0）
- ✅ Docker 镜像构建（`docker-compose build`）
- ✅ Docker 容器启动（`docker-compose up -d`）
- ✅ 项目构建（通过 Make 在容器中执行）
- ✅ 应用运行（Python 示例执行）
- ✅ 单元测试执行（pytest 在容器中）
- ✅ 构建清理（`make clean`、Docker 清理）

### 可选功能（P1-P2）
- ⚠️ Docker Shell 进入（可保留在 run.sh 中作为 `--shell` 参数）
- ⚠️ 配置文件指定（run.sh 支持参数传递）
- ⚠️ 静态分析（lint）：可集成到 build.sh 或 test.sh

### 移除功能（Out of Scope）
- ❌ 调试服务器启动（debug*.ps1）：使用 VSCode launch.json
- ❌ Docker 容器日志查看：直接使用 `docker-compose logs`
- ❌ Docker 容器重启/停止：直接使用 `docker-compose restart/down`
- ❌ 内存检查（Valgrind）：使用 VSCode tasks
- ❌ 详细的环境检测和诊断

**决策**:
3 个核心脚本实现如下功能：

**build.sh**:
```bash
# 功能：构建项目（Docker 镜像 + C/Python 编译）
# 参数：--clean（清理构建产物）
# 依赖：docker-compose、make
```

**run.sh**:
```bash
# 功能：运行应用
# 参数：[config_file]（默认 config/pid_temperature.yaml）
#      --shell（进入容器 Shell）
# 依赖：docker-compose、已构建的镜像
```

**test.sh**:
```bash
# 功能：执行单元测试和静态分析
# 参数：--lint（仅运行静态分析）
# 依赖：docker-compose、pytest
```

**迁移映射表**:
| PowerShell 脚本 | Bash 脚本 | 说明 |
|-----------------|----------|------|
| build.ps1 | build.sh | 核心功能迁移 |
| build.ps1 -Clean | build.sh --clean | 参数支持 |
| run.ps1 | run.sh | 核心功能迁移 |
| run.ps1 -Shell | run.sh --shell | 保留 Shell 模式 |
| debug*.ps1 | **移除** | 使用 IDE 调试 |
| scripts/debug-container.* | **移除** | 使用 tasks.json |
| docker-compose 直接命令 | **建议直接使用** | 不封装 |
| test_*.sh（现有） | test.sh | 统一入口 |

---

## 决策 5: 脚本输出格式

**问题**: 如何平衡简洁输出和必要信息？

**研究发现**:
- PowerShell 脚本使用 `Write-Host` 输出彩色日志（INFO/ERROR）
- Bash 可使用 ANSI 颜色码（`\033[32m` 绿色，`\033[31m` 红色）
- 但颜色码在日志重定向时会产生乱码

**决策**:
- 使用简单的前缀标记：`[INFO]`、`[ERROR]`、`[WARN]`
- 仅在检测到 TTY 时使用颜色（`[ -t 1 ]`）
- 输出格式：`[级别] 操作描述`，无时间戳（简化）
- 关键步骤输出进度，避免冗长日志

**示例**:
```bash
#!/usr/bin/env bash
set -e

# 颜色定义（仅在 TTY 时生效）
if [ -t 1 ]; then
    GREEN='\033[0;32m'
    RED='\033[0;31m'
    NC='\033[0m'  # No Color
else
    GREEN=''
    RED=''
    NC=''
fi

info() {
    echo -e "${GREEN}[INFO]${NC} $*"
}

error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}
```

---

## 决策 6: 项目结构影响

**问题**: 新脚本应放置在哪里？是否需要修改目录结构？

**当前结构**:
```
py-embedded-control/
├── build.ps1, run.ps1, debug*.ps1  (根目录)
├── scripts/
│   ├── debug-container.ps1
│   └── ...
└── tests/
    ├── test_*.sh
    └── ...
```

**决策**:
- 新脚本放置在**项目根目录**（与 PowerShell 同级）
- 文件命名：`build.sh`、`run.sh`、`test.sh`
- **不创建** scripts/bash/ 子目录（避免增加层级）
- 测试脚本仍在 `tests/` 目录，由 test.sh 调用
- **移除** scripts/ 中的 PowerShell 调试脚本

**过渡期**:
- 暂时保留 .ps1 和 .bat 文件（作为参考，1-2 周后删除）
- 在 README.md 中添加迁移说明

---

## 决策 7: 文档更新范围

**问题**: 哪些文档需要更新以反映脚本迁移？

**必须更新的文档**:
1. **README.md**: 更新"快速开始"部分，说明 Bash 环境要求
2. **docs/guides/WINDOWS_QUICKSTART.md**: 新增 Git Bash/WSL 安装指引
3. **PROJECT_FILES.md**: 更新脚本文件清单

**新增文档**:
4. **MIGRATION_FROM_POWERSHELL.md**: PowerShell 到 Bash 的迁移指南（放在根目录）

**可选更新**:
- docs/guides/DEBUGGING.md: 说明不再提供 debug.sh，使用 VSCode
- docs/guides/CONTAINER_DEBUG_SETUP.md: 更新脚本引用

**文档简化原则**:
- 每个脚本的 `--help` 参数提供基本用法
- README.md 只保留最常用的 3 个命令示例
- 详细文档仅在 MIGRATION_FROM_POWERSHELL.md 中说明

---

## 决策总结

| 决策点 | 选择方案 | 理由 |
|--------|---------|------|
| 跨平台路径 | 相对路径 + Git Bash 自动转换 | 最简洁，无需额外处理 |
| Bash 版本 | 严格兼容 Bash 3.2 | 最大兼容性（macOS） |
| 代码简化 | 移除环境检测+调试脚本，减少 81% 代码 | 符合极简设计目标 |
| 功能迁移 | 3 个核心脚本，移除 7+ 个辅助脚本 | 聚焦核心工作流 |
| 输出格式 | 简单前缀 + TTY 颜色检测 | 平衡可读性和简洁性 |
| 项目结构 | 根目录放置，不新增子目录 | 保持现有结构 |
| 文档更新 | 更新 3 个文档 + 新增 1 个迁移指南 | 最小必要更新 |

## 下一步（Phase 1）

- [ ] 生成 data-model.md（脚本参数和配置模型）
- [ ] 生成 contracts/（脚本接口规范）
- [ ] 生成 quickstart.md（开发者快速上手指南）
