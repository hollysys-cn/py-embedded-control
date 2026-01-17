# 项目文件说明

本文档说明项目根目录中各文件和目录的用途。

## 📂 目录结构

### 核心源代码

| 目录/文件 | 说明 |
|----------|------|
| `src/` | C 语言源代码（运行时、功能块、Python 绑定） |
| `python/` | Python 包和示例脚本 |
| `tests/` | 测试套件（单元测试、集成测试、基准测试） |

### 配置和构建

| 文件 | 说明 |
|------|------|
| `Makefile` | 主构建系统（make all/build/test/clean/lint） |
| `setup.py` | Python 扩展构建配置 |
| `pyproject.toml` | Python 项目元数据 |
| `requirements.txt` | Python 依赖清单 |

### Docker 环境

| 文件 | 说明 |
|------|------|
| `Dockerfile` | Docker 镜像定义（开发和生产环境） |
| `docker-compose.yml` | 容器编排配置（x86_64 和 ARM64） |
| `.dockerignore` | Docker 构建排除文件 |

### 脚本工具

| 文件 | 说明 |
|------|------|
| `start-debug.ps1` | Windows 一键启动调试脚本（常用）⭐ |
| `start-debug.sh` | Linux/macOS 一键启动调试脚本（常用）⭐ |
| `build.ps1` | Windows 构建脚本 |
| `run.ps1` | Windows 运行脚本 |
| `scripts/` | 辅助脚本目录（版权检查、验证、容器调试等）|

### 配置目录

| 目录 | 说明 |
|------|------|
| `config/` | 运行时配置文件模板 |
| `.vscode/` | VS Code 调试和任务配置 |
| `.github/` | GitHub Actions 和项目模板 |
| `.specify/` | Speckit 规格说明工具配置 |

### 文档

| 目录/文件 | 说明 |
|----------|------|
| `README.md` | 项目主文档（从这里开始！） |
| `LICENSE` | MIT 许可证文件 |
| `docs/` | 文档中心（指南、API、报告） |
| `docs/README.md` | 文档导航索引 |
| `docs/guides/` | 用户指南（快速开始、调试等） |
| `docs/reports/` | 技术报告和验证文档 |

### 规格说明

| 目录 | 说明 |
|------|------|
| `specs/` | 功能规格说明和实施计划 |
| `specs/001-plcopen-python-runtime/` | 主功能规格 |

### 构建产物（自动生成，已忽略）

| 目录/文件 | 说明 |
|----------|------|
| `bin/` | 编译后的可执行文件 |
| `build/` | Python 扩展构建临时文件 |
| `*.so` | 编译后的共享库（Python C 扩展） |
| `runtime.log` | 运行时日志文件 |

### 版本控制

| 文件 | 说明 |
|------|------|
| `.gitignore` | Git 忽略文件配置 |
| `.gitattributes` | Git 文件属性（编码、行尾） |
| `.flake8` | Python 代码检查配置 |

## 🚀 常用操作

### 快速开始
```bash
# 查看主文档
cat README.md

# 查看文档导航
cat docs/README.md

# Windows 一键调试（最常用！）
.\start-debug.ps1

# Linux/macOS 一键调试
./start-debug.sh

# 容器调试（辅助脚本）
.\scripts\debug-container.ps1    # Windows
./scripts/debug-container.sh     # Linux/macOS
```

### 构建和测试
```bash
# 完整构建
make all

# 运行测试
make test

# 代码检查
make lint
```

### 文档查阅
- 主文档: `README.md`
- 文档索引: `docs/README.md`
- 快速开始: `docs/guides/WINDOWS_QUICKSTART.md` 或 `docs/guides/LOCAL_SETUP.md`
- 远程调试: `docs/guides/REMOTE_DEBUG_QUICKSTART.md`
- API 参考: `docs/api_reference.md`

## 📌 重要提示

### 不要修改的文件
- `build/` - 自动生成
- `bin/` - 自动生成
- `*.so` - 编译产物
- `runtime.log` - 运行时生成

### 配置文件优先级
1. 用户配置: `config/*.yaml`（你的配置）
2. 示例模板: `config/*.yaml.example`（参考模板）

### 调试端口
- Python debugpy: 5678（x86_64）、5679（ARM64）
- C gdbserver: 9000（x86_64）、9001（ARM64）

## 🔍 查找文件

### 我想找...

| 需求 | 文件位置 |
|------|----------|
| 功能块实现 | `src/function_blocks/fb_*.c` |
| Python API | `python/plcopen/blocks.py` |
| 示例代码 | `python/examples/*.py` |
| 测试用例 | `tests/unit/` 或 `tests/integration/` |
| 配置模板 | `config/*.yaml.example` |
| 用户手册 | `docs/user_guide.md` |
| API 文档 | `docs/api_reference.md` |
| 调试指南 | `docs/guides/REMOTE_DEBUG_QUICKSTART.md` |
| 技术报告 | `docs/reports/` |

---

**最后更新**: 2026-01-17
**维护**: 项目根目录已优化，文档已重新组织
