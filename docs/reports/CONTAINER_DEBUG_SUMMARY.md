# 容器远程调试 - 完成总结

## 📦 已完成的配置

本次更新为 PLCopen Python Runtime 项目添加了完整的容器远程调试支持，可以在 Docker 容器中运行代码并使用 VS Code 进行高效的远程调试。

### 1. Docker 配置增强

#### Dockerfile 增强
- ✅ 添加调试工具：gdb, strace, valgrind, vim, procps, net-tools
- ✅ 配置调试环境变量：PYTHONUNBUFFERED, PYTHONDONTWRITEBYTECODE
- ✅ 优化 Python 和 C/C++ 开发环境

#### docker-compose.yml 优化
- ✅ 配置 Python 调试端口（5678/5679）
- ✅ 配置 C/C++ 调试端口（9000/9001）
- ✅ 添加 SYS_PTRACE 权限支持 gdb/valgrind
- ✅ 配置 seccomp:unconfined 支持调试工具
- ✅ 支持 x86_64 和 ARM64 双平台

### 2. VS Code 调试配置

#### launch.json 配置
- ✅ **Python 调试**:
  - 本地调试配置
  - Docker 容器调试（x86_64 和 ARM64）
  - 远程设备调试
  - 直接启动示例调试

- ✅ **C/C++ 调试**:
  - 本地 gdb 调试
  - gdbserver 远程调试

- ✅ **混合调试**:
  - Python + C++ 复合调试配置

#### tasks.json 配置
- ✅ Docker 容器管理任务（启动、停止、重启）
- ✅ 容器内构建任务
- ✅ 容器内测试任务
- ✅ 调试服务器启动任务
- ✅ Valgrind 内存检查任务

### 3. Dev Container 配置

#### .devcontainer/devcontainer.json
- ✅ 完整的 VS Code Dev Container 配置
- ✅ 自动安装推荐扩展（Python, C++, Docker, YAML）
- ✅ 端口自动转发和标签
- ✅ 容器构建后自动安装依赖
- ✅ 预配置的编辑器设置

### 4. 快速启动脚本

#### debug-container.ps1 (Windows)
- ✅ 支持 x86_64 和 ARM64 平台选择
- ✅ 可选重新构建镜像
- ✅ 可选运行示例并等待调试器
- ✅ 彩色输出和详细状态信息
- ✅ 错误处理和友好提示

#### debug-container.sh (Linux/macOS)
- ✅ 与 PowerShell 脚本功能相同
- ✅ 跨平台支持
- ✅ 命令行参数解析

### 5. 完整文档

#### 主要文档
- ✅ **container-debug.md** - 完整的容器调试指南
  - 快速开始
  - Python 调试详解
  - C/C++ 调试详解
  - 混合调试说明
  - 故障排除指南
  - 进阶技巧

- ✅ **container-debug-quickstart.md** - 快速开始指南
  - 三步开始
  - 一键运行示例
  - 常见问题

- ✅ **container-debug-examples.md** - 实用调试示例
  - Python PID 调试示例
  - C 代码调试示例
  - 混合调试示例
  - Valgrind 内存检查
  - 性能分析
  - 远程设备调试

- ✅ **CONTAINER_DEBUG_SETUP.md** - 配置验证文档
  - 配置文件清单
  - 调试配置总览
  - 功能测试清单

#### 更新的文档
- ✅ README.md - 添加容器调试文档链接

### 6. 配置文件更新

#### .gitignore
- ✅ 添加调试日志忽略规则
- ✅ 添加 Docker 临时文件忽略规则
- ✅ 添加 Dev Container 环境文件忽略规则

## 🎯 支持的调试场景

### Python 调试
| 场景 | 配置 | 说明 |
|------|------|------|
| 本地调试 | Python: 附加到运行时（本地） | 调试本地运行的 Python 代码 |
| 容器调试 (x86) | Python: 附加到 Docker 容器 (x86_64) | 调试 x86_64 容器中的代码 |
| 容器调试 (ARM) | Python: 附加到 Docker 容器 (ARM64) | 调试 ARM64 容器中的代码 |
| 启动调试 | Python: 启动 PID 温度控制示例 | 直接启动并调试示例 |
| 远程调试 | Python: 附加到远程设备 | 调试部署到设备的代码 |

### C/C++ 调试
| 场景 | 配置 | 说明 |
|------|------|------|
| 本地调试 | C++: 调试运行时 (gdb) | 使用本地 gdb 调试 |
| 远程调试 | C++: 附加到容器中的进程 (gdbserver) | 使用 gdbserver 远程调试 |

### 混合调试
| 场景 | 配置 | 说明 |
|------|------|------|
| Python + C | 混合调试: Python + C++ | 同时调试 Python 和 C 代码 |

## 📊 端口映射

```
主机                     容器                      用途
──────────────────────────────────────────────────────────
localhost:5678    →    plcopen-dev:5678         Python debugpy (x86_64)
localhost:9000    →    plcopen-dev:9000         C gdbserver (x86_64)
localhost:5679    →    plcopen-arm-test:5678    Python debugpy (ARM64)
localhost:9001    →    plcopen-arm-test:9000    C gdbserver (ARM64)
```

## 🚀 快速开始

### 方式 1: 使用快速启动脚本（推荐）

**Windows**:
```powershell
# 启动容器
.\debug-container.ps1

# 运行示例并等待调试器
.\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
```

**Linux/macOS**:
```bash
# 启动容器
./debug-container.sh

# 运行示例并等待调试器
./debug-container.sh -e pid_temperature -w
```

### 方式 2: 使用 Docker Compose

```bash
# 启动容器
docker-compose up -d dev

# 进入容器
docker exec -it plcopen-dev bash

# 启动调试服务器
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py
```

然后在 VS Code 中按 `F5`，选择 "Python: 附加到 Docker 容器 (x86_64)"。

### 方式 3: 使用 VS Code Dev Container

1. 安装扩展: `Remote - Containers`
2. 按 `F1` → 选择 "Remote-Containers: Reopen in Container"
3. 等待容器构建完成
4. 直接在容器中编辑和调试

## 🛠️ VS Code 任务

按 `Ctrl+Shift+P` → "Tasks: Run Task" 可使用以下任务：

### Docker 管理
- **Docker: 启动开发容器** - 启动 dev 容器
- **Docker: 停止开发容器** - 停止 dev 容器
- **Docker: 重启开发容器** - 重启 dev 容器
- **Docker: 构建并启动容器** - 重新构建并启动
- **Docker: 查看容器日志** - 实时查看日志
- **Docker: 进入容器 Shell** - 打开容器 bash

### 开发任务
- **容器: 构建项目** - 在容器中构建项目
- **容器: 运行 PID 示例** - 运行温度控制示例
- **容器: 运行单元测试** - 执行单元测试

### 调试任务
- **容器: 启动 Python 调试服务器** - 启动 debugpy
- **容器: 启动 gdbserver (C 调试)** - 启动 C 调试服务器
- **容器: 运行内存检查 (Valgrind)** - 检查内存泄漏

## 📚 文档导航

### 开始使用
1. **[快速开始指南](docs/container-debug-quickstart.md)** - 三步开始调试
2. **[完整调试指南](docs/container-debug.md)** - 详细的调试说明

### 实践
3. **[调试示例](docs/container-debug-examples.md)** - 7 个实用调试示例
4. **[配置验证](docs/CONTAINER_DEBUG_SETUP.md)** - 验证配置是否正确

### 参考
5. **[launch.json](.vscode/launch.json)** - 调试配置
6. **[tasks.json](.vscode/tasks.json)** - VS Code 任务
7. **[docker-compose.yml](docker-compose.yml)** - 容器配置
8. **[devcontainer.json](.devcontainer/devcontainer.json)** - Dev Container 配置

## 🎓 调试技巧

### Python 调试
- ✅ 使用条件断点：`temperature > 80.0`
- ✅ 使用日志断点：`温度: {temperature}, 输出: {output}`
- ✅ 在调试控制台执行代码：`pid.kp = 5.0`
- ✅ 设置 `justMyCode: false` 调试库代码

### C/C++ 调试
- ✅ 查看变量：`print *pid`
- ✅ 查看内存：`x/20xw pid`
- ✅ 设置监视点：`watch pid->integral`
- ✅ 条件断点：`break pid_update if error > 10.0`

### 性能调试
- ✅ 使用 Valgrind 检查内存泄漏
- ✅ 使用 cProfile 分析 Python 性能
- ✅ 使用 strace 追踪系统调用
- ✅ 使用 time 命令测量执行时间

## 🔧 故障排除

### 常见问题

1. **无法连接到调试端口**
   - 检查容器是否运行：`docker ps | grep plcopen`
   - 检查端口映射：`docker port plcopen-dev`

2. **断点未命中**
   - 检查路径映射是否正确
   - 重新加载窗口：`Ctrl+Shift+P` → `Reload Window`

3. **gdb 权限被拒绝**
   - 确认容器有 SYS_PTRACE 权限
   - 重新创建容器：`docker-compose down && docker-compose up -d`

完整的故障排除指南请参考 [container-debug.md](docs/container-debug.md#故障排除)。

## 🎉 总结

本次更新提供了：

✅ **完整的容器调试环境**
- Docker 配置优化
- VS Code 调试配置
- Dev Container 支持

✅ **便捷的启动工具**
- Windows PowerShell 脚本
- Linux/macOS Bash 脚本
- VS Code 任务集成

✅ **详尽的文档**
- 快速开始指南
- 完整调试指南
- 实用调试示例
- 配置验证文档

✅ **多平台支持**
- x86_64 平台
- ARM64 平台
- 本地和远程调试

✅ **多语言调试**
- Python 调试
- C/C++ 调试
- 混合调试

现在你可以在容器中高效地开发和调试 PLCopen Python Runtime 项目了！🚀

---

**需要帮助？**
- 查看 [常见问题](docs/faq.md)
- 查看 [故障排除指南](docs/container-debug.md#故障排除)
- 提交 [Issue](https://github.com/hollysys-cn/py-embedded-control/issues)
