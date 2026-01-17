# 快速开始: 统一 Bash 脚本开发

**功能**: 统一使用 Bash 脚本实现跨平台开发 | **日期**: 2026年1月18日

## 概述

本指南帮助开发者快速上手使用新的 Bash 脚本进行跨平台开发。无论你使用 Windows、macOS 还是 Linux，都将使用相同的 3 个脚本完成所有开发任务。

## 前置条件

### 所有平台共同要求

- Git（用于克隆项目）
- Docker 和 Docker Compose
- 基本的命令行操作能力

### 平台特定要求

#### Windows

**选择以下任一方案**:

**方案 A: Git Bash**（推荐）
1. 下载并安装 [Git for Windows](https://gitforwindows.org/)
2. 安装时选择 "Git Bash Here" 选项
3. 在项目目录右键选择 "Git Bash Here" 打开终端

**方案 B: WSL（Windows Subsystem for Linux）**
1. 启用 WSL: `wsl --install`
2. 安装 Ubuntu: `wsl --install -d Ubuntu`
3. 在 WSL 终端中操作

#### macOS

- 默认的 Terminal.app 或 iTerm2 即可
- Bash 3.2+ 已预装（无需额外安装）

#### Linux

- 默认终端即可
- Bash 4.0+ 通常已预装

## 5 分钟快速开始

### 1. 克隆项目（1 分钟）

```bash
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control
```

### 2. 切换到功能分支（30 秒）

```bash
git checkout 002-unified-bash-scripts
```

### 3. 构建项目（2-3 分钟）

```bash
./build.sh
```

**预期输出**:
```
[INFO] Building Docker image...
[INFO] Docker image built successfully
[INFO] Building project in container...
[INFO] Build completed successfully
```

### 4. 运行示例（30 秒）

```bash
./run.sh
```

**预期输出**:
```
[INFO] Starting application with config: config/pid_temperature.yaml
[INFO] Container is ready
[2026-01-18 10:30:00] INFO: PLCopen Runtime starting...
```

### 5. 运行测试（30 秒）

```bash
./test.sh
```

**预期输出**:
```
[INFO] Running static analysis...
[INFO] Static analysis passed
[INFO] Running unit tests...
============================== 25 passed in 0.50s ==============================
[INFO] All tests passed
```

**✅ 完成！** 你已经成功在你的平台上运行了项目。

---

## 核心脚本说明

### build.sh - 构建项目

**用途**: 构建 Docker 镜像和编译 C/Python 代码

**常用命令**:
```bash
./build.sh          # 标准构建
./build.sh --clean  # 清理后构建（CI/CD 推荐）
./build.sh --help   # 查看帮助
```

**何时使用**:
- 首次设置项目
- 修改 C 代码或构建配置后
- CI/CD 流程中

---

### run.sh - 运行应用

**用途**: 在 Docker 容器中运行 PLCopen Runtime

**常用命令**:
```bash
./run.sh                        # 使用默认配置
./run.sh config/custom.yaml     # 使用自定义配置
./run.sh --shell                # 进入容器 Shell 调试
./run.sh --help                 # 查看帮助
```

**何时使用**:
- 运行应用程序
- 测试不同配置
- 需要进入容器调试时

---

### test.sh - 运行测试

**用途**: 执行静态分析和单元测试

**常用命令**:
```bash
./test.sh           # 运行所有测试
./test.sh --lint    # 仅静态分析
./test.sh --unit    # 仅单元测试
./test.sh --help    # 查看帮助
```

**何时使用**:
- 提交代码前验证质量
- CI/CD 流程中
- 重构代码后验证

---

## 常见工作流

### 开发新功能

```bash
# 1. 更新代码
git pull

# 2. 构建（如有依赖变更）
./build.sh

# 3. 运行测试
./test.sh

# 4. 开发...（修改代码）

# 5. 增量构建（如修改了 C 代码）
./build.sh

# 6. 运行应用验证
./run.sh

# 7. 再次测试
./test.sh

# 8. 提交代码
git add .
git commit -m "feat: 新功能描述"
```

### 调试问题

```bash
# 1. 进入容器 Shell
./run.sh --shell

# 2. 在容器内手动执行命令
root@container:/workspace# python3 python/examples/pid_temperature.py
root@container:/workspace# gdb bin/plcopen_runtime

# 3. 退出容器
root@container:/workspace# exit

# 4. 查看日志（如果应用生成了日志文件）
cat runtime.log
```

### 清理和重建

```bash
# 完全清理并重建
./build.sh --clean

# 停止 Docker 容器
docker-compose down

# 查看 Docker 容器状态
docker-compose ps

# 重启 Docker 容器
docker-compose restart dev
```

---

## 与 PowerShell 脚本的对比

如果你之前使用 PowerShell 脚本，以下是迁移映射：

| 旧命令（PowerShell） | 新命令（Bash） | 说明 |
|---------------------|---------------|------|
| `.\build.ps1` | `./build.sh` | 功能相同 |
| `.\build.ps1 -Clean` | `./build.sh --clean` | 参数改为 -- 前缀 |
| `.\run.ps1` | `./run.sh` | 功能相同 |
| `.\run.ps1 -Shell` | `./run.sh --shell` | 参数改为 -- 前缀 |
| `.\debug*.ps1` | **移除** | 使用 VSCode 调试或 `./run.sh --shell` |
| `docker-compose up -d dev` | 直接使用 | 脚本内部自动调用 |
| 各种测试脚本 | `./test.sh` | 统一入口 |

**主要变化**:
1. 脚本扩展名：`.ps1` → `.sh`
2. 执行前缀：`.\` → `./`
3. 参数格式：`-Parameter` → `--parameter`
4. 脚本数量：10+ → 3 个

---

## 疑难解答

### Windows: 脚本无法执行

**问题**: `bash: ./build.sh: Permission denied`

**解决方案**:
```bash
# 给脚本添加执行权限
chmod +x build.sh run.sh test.sh

# 或者使用 bash 命令显式执行
bash build.sh
```

### macOS: Bash 版本警告

**问题**: 提示 Bash 版本过旧

**解决方案**: 脚本已兼容 Bash 3.2，忽略警告即可。或升级 Bash：
```bash
brew install bash
```

### Docker 容器无法启动

**问题**: `ERROR: Couldn't connect to Docker daemon`

**解决方案**:
1. 确认 Docker Desktop 正在运行
2. 检查 Docker 权限：
   ```bash
   # Linux: 添加用户到 docker 组
   sudo usermod -aG docker $USER
   # 注销并重新登录
   ```

### 构建失败

**问题**: 编译错误或依赖问题

**解决方案**:
1. 清理并重建：
   ```bash
   ./build.sh --clean
   ```

2. 如仍失败，清理 Docker 缓存：
   ```bash
   docker-compose down
   docker system prune -f
   ./build.sh --clean
   ```

### Git Bash 路径问题

**问题**: Windows 路径格式错误（如 `C:\path` vs `/c/path`）

**解决方案**: Git Bash 会自动转换，无需手动处理。确保：
- 使用相对路径（`./config/file.yaml`）
- 或使用 `$(pwd)` 获取当前目录

---

## 性能优化提示

### 加速 Docker 构建

```bash
# 启用 BuildKit（在 .env 文件中）
echo "DOCKER_BUILDKIT=1" >> .env

# 或临时启用
DOCKER_BUILDKIT=1 ./build.sh
```

### 减少增量构建时间

- 仅修改 Python 代码时无需重新运行 `build.sh`
- 修改配置文件时直接运行 `./run.sh`

### 并行测试（高级）

```bash
# 静态分析和单元测试并行运行
./test.sh --lint & ./test.sh --unit & wait
```

---

## 下一步

- 阅读 [data-model.md](data-model.md) 了解脚本参数详情
- 阅读 [contracts/](contracts/) 了解脚本接口契约
- 阅读 [MIGRATION_FROM_POWERSHELL.md](../../../MIGRATION_FROM_POWERSHELL.md) 了解详细迁移指南（待创建）

## 获取帮助

每个脚本都支持 `--help` 参数：

```bash
./build.sh --help
./run.sh --help
./test.sh --help
```

如有问题，请查看项目文档或提交 Issue。
