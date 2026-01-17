# 本地开发环境安装指南

**日期**: 2026年1月17日
**系统**: Windows 11
**检测状态**: ✅ 基础环境完整

## 环境检测结果

### ✅ 已安装组件

| 组件 | 版本 | 状态 | 路径 |
|------|------|------|------|
| **Docker Desktop** | 29.1.3 | ✅ 运行中 | `C:\Program Files\Docker\Docker\` |
| **Git** | 2.52.0 | ✅ 可用 | `C:\Program Files\Git\` |
| **VS Code** | 1.108.1 | ✅ 可用 | `C:\Users\guog\AppData\Local\Programs\Microsoft VS Code\` |
| **Python** | 3.13.11 | ✅ 可用 | 系统全局 |
| **docker-compose** | v2.x (内置) | ✅ 可用 | Docker Desktop 集成 |

### 🎉 结论

**您的开发环境已完全就绪！** 无需额外安装任何工具。

## 项目依赖验证

根据项目要求（[plan.md](specs/001-plcopen-python-runtime/plan.md)），需要以下环境：

### 必需组件（容器内提供）

以下组件在 Docker 容器中自动提供，**无需本地安装**：

- ✅ **GCC/G++** (C11 编译器) - 容器内版本
- ✅ **Python 3.13** (运行时) - 容器内版本
- ✅ **Make** (构建工具) - 容器内版本
- ✅ **cppcheck** (静态分析) - 容器内版本
- ✅ **Python 包**:
  - `debugpy` (远程调试)
  - `pyyaml` (配置文件解析)
  - `pytest` (单元测试)
  - `flake8` (代码检查)

### 可选组件

| 组件 | 用途 | 状态 | 说明 |
|------|------|------|------|
| **Python 3.8+** (本地) | 脚本开发、IDE 支持 | ✅ 3.13.11 已安装 | 可用于本地开发 |
| **VS Code 扩展** | 开发体验增强 | ⚠️ 建议安装 | 见下方列表 |
| **PowerShell 7+** | 脚本执行 | ⚠️ 使用内置版本 | Windows 11 自带 |

## VS Code 推荐扩展

为了获得最佳开发体验，建议安装以下 VS Code 扩展：

### 必需扩展

```bash
# Python 开发
code --install-extension ms-python.python
code --install-extension ms-python.vscode-pylance

# Docker 支持
code --install-extension ms-azuretools.vscode-docker

# C/C++ 支持（用于查看 C 源码）
code --install-extension ms-vscode.cpptools

# 调试支持
code --install-extension ms-python.debugpy
```

### 推荐扩展

```bash
# YAML 语法高亮（配置文件）
code --install-extension redhat.vscode-yaml

# Markdown 预览增强
code --install-extension yzhang.markdown-all-in-one

# Git 增强
code --install-extension eamodio.gitlens

# 代码格式化
code --install-extension esbenp.prettier-vscode
```

### 一键安装（PowerShell）

```powershell
# 复制并运行以下命令
$extensions = @(
    "ms-python.python",
    "ms-python.vscode-pylance",
    "ms-azuretools.vscode-docker",
    "ms-vscode.cpptools",
    "ms-python.debugpy",
    "redhat.vscode-yaml",
    "yzhang.markdown-all-in-one",
    "eamodio.gitlens",
    "esbenp.prettier-vscode"
)

foreach ($ext in $extensions) {
    Write-Host "安装扩展: $ext" -ForegroundColor Green
    code --install-extension $ext
}

Write-Host "所有扩展安装完成！" -ForegroundColor Green
```

## 快速验证

运行以下命令验证环境：

```bash
# 1. 验证 Docker
docker --version
docker ps

# 2. 验证 Git
git --version

# 3. 验证 VS Code
code --version

# 4. 验证 Python（可选）
python --version

# 5. 构建项目
cd C:\Users\guog\github\hollysys-cn\py-embedded-control
docker-compose build dev

# 6. 运行测试
docker-compose run --rm dev python3 test_module.py
```

## 开始开发

### 方式 1：使用 PowerShell 脚本（推荐）

```powershell
# 构建项目
.\build.ps1 -All

# 运行示例
.\run.ps1

# 运行调试模式
.\run.ps1 -Debug

# 进入开发 Shell
.\run.ps1 -Shell
```

**注意**: 如果遇到 PowerShell 执行策略错误，运行：
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

### 方式 2：使用 Docker Compose 直接命令

```bash
# 构建镜像
docker-compose build dev

# 构建运行时
docker-compose run --rm dev bash -c "cd /workspace && make runtime"

# 构建 Python 扩展
docker-compose run --rm dev bash -c "cd /workspace && python3 setup.py build_ext --inplace"

# 运行测试
docker-compose run --rm dev python3 test_module.py

# 运行示例
docker-compose run --rm dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
```

## 常见问题

### Docker Desktop 未启动

**症状**:
```
error during connect: This error may indicate that the docker daemon is not running
```

**解决**:
1. 启动 Docker Desktop
2. 等待 Docker 图标变为绿色
3. 重试命令

### PowerShell 脚本执行被阻止

**症状**:
```
无法加载文件 build.ps1，因为在此系统上禁止运行脚本
```

**解决**:
```powershell
# 临时允许执行（推荐）
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

# 或永久更改（管理员权限）
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

### 端口冲突（调试模式）

**症状**:
```
Error: Port 5678 is already in use
```

**解决**:
1. 查找占用进程: `netstat -ano | findstr 5678`
2. 终止进程: `taskkill /PID <PID> /F`
3. 或修改配置文件中的调试端口

### Git Bash vs PowerShell

本项目的 PowerShell 脚本（`build.ps1`, `run.ps1`）在 **PowerShell** 中运行，而不是 Git Bash。

**如果使用 Git Bash**:
```bash
# 直接使用 docker-compose 命令
docker-compose build dev
docker-compose run --rm dev bash
```

## 本地开发工作流

### 1. 修改 C 代码

```bash
# 编辑 C 源文件
code src/function_blocks/fb_pid.c

# 重新编译
docker-compose run --rm dev bash -c "cd /workspace && make clean && make runtime"

# 测试
docker-compose run --rm dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
```

### 2. 修改 Python 绑定

```bash
# 编辑 Python 绑定
code src/python_bindings/py_pid.c

# 重新编译 Python 扩展
docker-compose run --rm dev bash -c "cd /workspace && python3 setup.py build_ext --inplace"

# 测试
docker-compose run --rm dev python3 test_module.py
```

### 3. 修改 Python 脚本

```bash
# 编辑 Python 控制脚本
code python/examples/pid_temperature.py

# 直接运行（无需重新编译）
docker-compose run --rm dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
```

### 4. 调试 Python 脚本

```bash
# 1. 启动调试模式
.\run.ps1 -Debug

# 2. 在 VS Code 中按 F5，选择 "Python: 附加到运行时"

# 3. 设置断点，开始调试
```

## 性能优化建议

### Docker Desktop 配置

1. **分配资源** (设置 → Resources)：
   - **CPU**: 至少 2 核（推荐 4 核）
   - **内存**: 至少 4GB（推荐 8GB）
   - **交换空间**: 2GB

2. **WSL 2 后端** (设置 → General)：
   - ✅ 启用 "Use the WSL 2 based engine"
   - 更好的文件系统性能

3. **文件共享优化**:
   - 项目位于用户目录下 (`C:\Users\guog\`)
   - 避免跨驱动器共享

### 加速构建

```bash
# 使用阿里云镜像（已在 Dockerfile 配置）
# 无需额外操作

# 使用 Docker 缓存
docker-compose build --no-cache  # 仅在必要时使用

# 并行构建（如果有多个服务）
docker-compose build --parallel
```

## 后续步骤

1. ✅ **环境验证完成** - 您已准备就绪
2. 📖 **阅读文档**: [README.md](README.md), [WINDOWS_QUICKSTART.md](WINDOWS_QUICKSTART.md)
3. 🚀 **运行示例**: `.\run.ps1`
4. 🔧 **开始开发**: 修改代码并测试
5. 🐛 **调试功能**: `.\run.ps1 -Debug`
6. 📋 **验证清单**: [VERIFICATION_CHECKLIST.md](VERIFICATION_CHECKLIST.md)

## 参考资料

- [项目 README](README.md)
- [Windows 快速开始](WINDOWS_QUICKSTART.md)
- [构建验证报告](BUILD_VERIFICATION.md)
- [验证检查清单](VERIFICATION_CHECKLIST.md)
- [项目完成报告](PROJECT_COMPLETION_REPORT.md)
- [技术规范](specs/001-plcopen-python-runtime/spec.md)
- [实施计划](specs/001-plcopen-python-runtime/plan.md)

---

**环境状态**: ✅ 完全就绪
**下一步**: 运行 `.\run.ps1` 启动示例程序
