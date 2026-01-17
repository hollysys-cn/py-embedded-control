# 容器远程调试配置验证

本文档验证所有容器远程调试配置已正确设置。

## ✅ 配置文件清单

### 1. Docker 配置

- ✅ [Dockerfile](../Dockerfile) - 已增强调试工具支持
  - 添加了 gdb, strace, valgrind, vim 等调试工具
  - 配置了 PYTHONUNBUFFERED 和 PYTHONDONTWRITEBYTECODE

- ✅ [docker-compose.yml](../docker-compose.yml) - 已优化调试配置
  - 配置了 debugpy 端口 (5678/5679)
  - 配置了 gdbserver 端口 (9000/9001)
  - 添加了 SYS_PTRACE 权限和 seccomp:unconfined
  - 支持 x86_64 和 ARM64 双平台

### 2. VS Code 配置

- ✅ [.vscode/launch.json](../.vscode/launch.json) - 完整的调试配置
  - Python 调试配置（本地、容器、远程）
  - C/C++ 调试配置（gdb、gdbserver）
  - 混合调试配置（Python + C++）
  - 支持启动和附加两种调试模式

- ✅ [.vscode/tasks.json](../.vscode/tasks.json) - 容器操作任务
  - Docker 容器启动/停止/重启
  - 容器内构建和测试
  - 启动调试服务器
  - 运行 Valgrind 内存检查

- ✅ [.vscode/settings.json](../.vscode/settings.json) - VS Code 设置

### 3. Dev Container 配置

- ✅ [.devcontainer/devcontainer.json](../.devcontainer/devcontainer.json)
  - VS Code Dev Container 完整配置
  - 自动安装推荐扩展
  - 端口转发配置
  - 容器构建后自动安装依赖

### 4. 快速启动脚本

- ✅ [debug-container.ps1](../../scripts/debug-container.ps1) - Windows PowerShell 脚本
  - 支持 x86_64 和 ARM64 平台
  - 可选择是否重新构建镜像
  - 可直接运行示例并等待调试器

- ✅ [debug-container.sh](../../scripts/debug-container.sh) - Linux/macOS Bash 脚本
  - 功能与 PowerShell 脚本相同
  - 跨平台支持

### 5. 文档

- ✅ [docs/container-debug.md](container-debug.md) - 完整的容器调试指南
  - Python 调试详细说明
  - C/C++ 调试详细说明
  - 混合调试说明
  - 故障排除指南

- ✅ [docs/container-debug-quickstart.md](container-debug-quickstart.md) - 快速开始指南

- ✅ [README.md](../README.md) - 已添加容器调试文档链接

### 6. 忽略文件

- ✅ [.gitignore](../.gitignore) - 已添加调试相关忽略规则
  - 调试日志
  - Docker 临时文件
  - Dev Container 环境文件

## 🎯 调试配置总览

| 调试场景 | 配置名称 | 端口 | 平台 |
|---------|---------|------|------|
| Python 本地调试 | Python: 附加到运行时（本地） | 5678 | 本地 |
| Python Docker x86 | Python: 附加到 Docker 容器 (x86_64) | 5678 | x86_64 |
| Python Docker ARM | Python: 附加到 Docker 容器 (ARM64) | 5679 | ARM64 |
| Python 启动示例 | Python: 启动 PID 温度控制示例 | - | 本地 |
| Python 远程设备 | Python: 附加到远程设备 | 5678 | 自定义 |
| C/C++ gdb 调试 | C++: 调试运行时 (gdb) | - | 本地 |
| C/C++ 远程调试 | C++: 附加到容器中的进程 (gdbserver) | 9000 | 容器 |
| 混合调试 | 混合调试: Python + C++ | 5678+9000 | 容器 |

## 🧪 功能测试清单

### 测试 1: 启动容器

**Windows**:
```powershell
.\scripts\debug-container.ps1
```

**Linux/macOS**:
```bash
./scripts/debug-container.sh
```

**预期结果**:
- ✅ 容器成功启动
- ✅ 显示容器信息（Python 和 GCC 版本）
- ✅ 项目在容器内构建成功

### 测试 2: Python 远程调试

**步骤**:
1. 启动容器：`docker-compose up -d dev`
2. 启动调试服务器：
   ```bash
   docker exec -it plcopen-dev python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client /workspace/python/examples/pid_temperature.py
   ```
3. 在 VS Code 中按 F5，选择 "Python: 附加到 Docker 容器 (x86_64)"

**预期结果**:
- ✅ VS Code 成功连接到调试服务器
- ✅ 可以设置断点
- ✅ 可以单步执行
- ✅ 可以查看变量值

### 测试 3: C/C++ 远程调试

**步骤**:
1. 启动容器
2. 启动 gdbserver：
   ```bash
   docker exec -it plcopen-dev gdbserver :9000 /workspace/bin/plcopen_runtime --config /workspace/config/pid_temperature.yaml
   ```
3. 在 VS Code 中选择 "C++: 附加到容器中的进程 (gdbserver)"

**预期结果**:
- ✅ gdb 成功连接
- ✅ 可以在 C 代码中设置断点
- ✅ 可以查看 C 变量和内存

### 测试 4: Dev Container

**步骤**:
1. 在 VS Code 中按 `F1`
2. 选择 "Remote-Containers: Reopen in Container"
3. 等待容器构建和配置完成

**预期结果**:
- ✅ VS Code 在容器中打开
- ✅ 推荐扩展自动安装
- ✅ Python 和 C++ 工具链可用
- ✅ 可以直接在容器中编辑和调试

### 测试 5: 混合调试

**步骤**:
1. 同时启动 debugpy 和 gdbserver
2. 选择 "混合调试: Python + C++"

**预期结果**:
- ✅ 两个调试会话同时运行
- ✅ 可以在 Python 和 C 代码之间切换调试

### 测试 6: VS Code 任务

**步骤**:
1. 按 `Ctrl+Shift+P`
2. 选择 "Tasks: Run Task"
3. 测试各个任务

**预期结果**:
- ✅ "Docker: 启动开发容器" - 容器启动成功
- ✅ "容器: 构建项目" - 项目构建成功
- ✅ "容器: 运行单元测试" - 测试执行成功
- ✅ "容器: 启动 Python 调试服务器" - 调试服务器启动

## 📊 调试端口映射

```
主机                     容器
────────────────────────────────
localhost:5678    ─►    plcopen-dev:5678      (Python debugpy)
localhost:9000    ─►    plcopen-dev:9000      (C gdbserver)
localhost:5679    ─►    plcopen-arm-test:5678 (Python debugpy ARM)
localhost:9001    ─►    plcopen-arm-test:9000 (C gdbserver ARM)
```

## 🔐 安全配置

容器调试需要以下特权：

```yaml
cap_add:
  - SYS_PTRACE          # 允许 ptrace 系统调用（gdb 需要）
security_opt:
  - seccomp:unconfined  # 允许所有系统调用（调试工具需要）
```

⚠️ **注意**: 这些权限仅用于开发环境，生产环境不应使用。

## 🎓 使用建议

### 推荐工作流程

1. **日常开发**: 使用 Dev Container
   - 在 VS Code 中重新打开到容器
   - 直接在容器中编辑和调试

2. **Python 调试**: 使用 debugpy 附加
   - 启动容器
   - 运行脚本时添加 debugpy
   - 在 VS Code 中附加调试器

3. **C 调试**: 使用 gdbserver
   - 启动 gdbserver 运行程序
   - 在 VS Code 中附加 gdb

4. **性能调试**: 使用 Valgrind
   - 运行 VS Code 任务 "容器: 运行内存检查"
   - 查看内存泄漏报告

### 最佳实践

- ✅ 使用快速启动脚本减少手动操作
- ✅ 利用 VS Code 任务快速执行常见操作
- ✅ 设置日志断点而不是 print 语句
- ✅ 使用条件断点提高调试效率
- ✅ 定期运行内存检查避免内存泄漏

## 🎉 结论

所有容器远程调试配置已完成并验证：

- ✅ Docker 环境配置完整
- ✅ VS Code 调试配置完整
- ✅ 快速启动脚本可用
- ✅ 文档完善
- ✅ 支持 Python、C/C++ 和混合调试
- ✅ 支持 x86_64 和 ARM64 双平台

现在可以开始在容器中进行高效的远程调试！🚀
