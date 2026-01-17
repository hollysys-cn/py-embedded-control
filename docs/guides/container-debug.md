# 容器远程调试指南

本文档介绍如何在 Docker 容器中运行 PLCopen Python Runtime 并使用 VS Code 进行远程调试。

## 📋 目录

- [快速开始](#快速开始)
- [调试配置详解](#调试配置详解)
- [Python 调试](#python-调试)
- [C/C++ 调试](#cc-调试)
- [混合调试（Python + C）](#混合调试python--c)
- [故障排除](#故障排除)

## 🚀 快速开始

### 方法 1: 使用快速启动脚本（推荐）

#### Windows (PowerShell)

```powershell
# 启动开发容器
.\debug-container.ps1

# 启动 ARM64 容器并重新构建镜像
.\debug-container.ps1 -Platform arm64 -BuildImage

# 运行示例并等待调试器连接
.\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
```

#### Linux/macOS (Bash)

```bash
# 给脚本添加执行权限
chmod +x debug-container.sh

# 启动开发容器
./debug-container.sh

# 启动 ARM64 容器并重新构建镜像
./debug-container.sh -p arm64 -b

# 运行示例并等待调试器连接
./debug-container.sh -e pid_temperature -w
```

### 方法 2: 使用 Docker Compose

```bash
# 启动 x86_64 开发容器
docker-compose up -d dev

# 启动 ARM64 测试容器
docker-compose up -d arm-test

# 查看容器日志
docker-compose logs -f dev

# 停止容器
docker-compose down
```

### 方法 3: 使用 VS Code Dev Container

1. 安装 VS Code 扩展：`Remote - Containers`
2. 按 `F1` 或 `Ctrl+Shift+P` 打开命令面板
3. 选择 `Remote-Containers: Reopen in Container`
4. VS Code 将自动构建并连接到容器

## 🔧 调试配置详解

### 端口映射

| 服务 | Python 调试端口 | C/C++ 调试端口 | 平台 |
|------|----------------|----------------|------|
| dev  | 5678           | 9000           | x86_64 |
| arm-test | 5679       | 9001           | ARM64 |

### 容器特权配置

为了支持 `gdb`、`valgrind` 等调试工具，容器需要以下特权：

```yaml
cap_add:
  - SYS_PTRACE
security_opt:
  - seccomp:unconfined
```

## 🐍 Python 调试

### 1. 在容器中启动 debugpy 服务器

```bash
# 进入容器
docker exec -it plcopen-dev bash

# 启动调试服务器并运行示例
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py
```

### 2. 在 VS Code 中连接调试器

1. 打开 VS Code 调试面板（`Ctrl+Shift+D`）
2. 选择配置：`Python: 附加到 Docker 容器 (x86_64)`
3. 按 `F5` 启动调试
4. 设置断点并观察变量

### 调试配置选项

#### 附加到运行中的进程

```json
{
    "name": "Python: 附加到 Docker 容器 (x86_64)",
    "type": "debugpy",
    "request": "attach",
    "connect": {
        "host": "localhost",
        "port": 5678
    },
    "pathMappings": [
        {
            "localRoot": "${workspaceFolder}",
            "remoteRoot": "/workspace"
        }
    ],
    "justMyCode": false
}
```

#### 启动并调试

```json
{
    "name": "Python: 启动 PID 温度控制示例",
    "type": "debugpy",
    "request": "launch",
    "program": "${workspaceFolder}/python/examples/pid_temperature.py",
    "console": "integratedTerminal",
    "justMyCode": false
}
```

### Python 调试技巧

#### 1. 条件断点

在断点上右键 → "编辑断点" → 添加条件：

```python
temperature > 80.0
```

#### 2. 日志断点

在断点上右键 → "添加日志点"：

```
温度: {temperature}, 输出: {output}
```

#### 3. 调试控制台

在调试时可以在控制台中执行 Python 代码：

```python
# 查看变量
print(pid.kp, pid.ki, pid.kd)

# 修改变量
pid.setpoint = 100.0

# 调用函数
pid.update(75.0, 0.1)
```

## 🔩 C/C++ 调试

### 1. 在容器中启动 gdbserver

```bash
# 进入容器
docker exec -it plcopen-dev bash

# 使用 gdbserver 启动程序
gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 2. 在 VS Code 中连接调试器

1. 打开调试面板
2. 选择配置：`C++: 附加到容器中的进程 (gdbserver)`
3. 按 `F5` 启动调试

### C/C++ 调试配置

```json
{
    "name": "C++: 附加到容器中的进程 (gdbserver)",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/bin/plcopen_runtime",
    "miDebuggerServerAddress": "localhost:9000",
    "miDebuggerPath": "/usr/bin/gdb",
    "MIMode": "gdb",
    "sourceFileMap": {
        "/workspace": "${workspaceFolder}"
    }
}
```

### C/C++ 调试技巧

#### 1. 查看内存

在调试控制台中：

```gdb
# 查看内存内容（16 字节）
x/16xb &pid->state

# 以浮点数格式查看
x/4f &pid->integral
```

#### 2. 断点命令

```gdb
# 在函数入口设置断点
break pid_update

# 条件断点
break pid_update if error > 10.0

# 断点时执行命令
commands 1
  print error
  print output
  continue
end
```

#### 3. 监视点

```gdb
# 当变量改变时暂停
watch pid->integral

# 读取时暂停
rwatch pid->setpoint

# 读写时都暂停
awatch pid->output
```

## 🔀 混合调试（Python + C）

VS Code 支持同时调试 Python 和 C/C++ 代码。

### 1. 启动混合调试

在容器中启动两个调试服务器：

```bash
# 终端 1: 启动 Python debugpy
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py &

# 终端 2: 启动 gdbserver（如果需要调试 C 扩展）
gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 2. 在 VS Code 中使用复合调试配置

选择配置：`混合调试: Python + C++`

这会同时启动两个调试器，你可以：
- 在 Python 代码中设置断点
- 在 C 代码中设置断点
- 跨语言追踪调用栈

### 复合调试配置

```json
{
    "name": "混合调试: Python + C++",
    "configurations": [
        "Python: 附加到 Docker 容器 (x86_64)",
        "C++: 附加到容器中的进程 (gdbserver)"
    ],
    "stopAll": true
}
```

## 🛠️ 故障排除

### 问题 1: 无法连接到调试端口

**症状**: VS Code 显示 "无法连接到调试服务器"

**解决方案**:

1. 检查容器是否运行：
   ```bash
   docker ps | grep plcopen
   ```

2. 检查端口映射：
   ```bash
   docker port plcopen-dev
   ```

3. 确认调试服务器正在监听：
   ```bash
   docker exec plcopen-dev netstat -tuln | grep 5678
   ```

4. 检查防火墙设置（Windows）：
   ```powershell
   # 允许 Docker 端口
   New-NetFirewallRule -DisplayName "Docker Debug" -Direction Inbound -Protocol TCP -LocalPort 5678 -Action Allow
   ```

### 问题 2: 断点未命中

**症状**: 设置的断点显示为灰色或未被触发

**解决方案**:

1. 检查路径映射（`pathMappings`）是否正确
2. 确认 `justMyCode` 设置为 `false`（如果要调试库代码）
3. 重新加载调试器配置：`Ctrl+Shift+P` → `Reload Window`

### 问题 3: 源代码路径不匹配

**症状**: 调试器停在断点但无法显示源代码

**解决方案**:

更新 `pathMappings`：

```json
"pathMappings": [
    {
        "localRoot": "${workspaceFolder}",
        "remoteRoot": "/workspace"
    }
]
```

### 问题 4: gdb 权限被拒绝

**症状**: `gdb` 或 `gdbserver` 提示权限错误

**解决方案**:

1. 确认容器使用了特权配置：
   ```bash
   docker inspect plcopen-dev | grep -A 5 CapAdd
   ```

2. 如果缺失，更新 `docker-compose.yml`：
   ```yaml
   cap_add:
     - SYS_PTRACE
   security_opt:
     - seccomp:unconfined
   ```

3. 重新创建容器：
   ```bash
   docker-compose down
   docker-compose up -d dev
   ```

### 问题 5: 调试速度很慢

**症状**: 单步执行或查看变量时响应缓慢

**解决方案**:

1. 禁用不必要的断点和监视点
2. 设置 `justMyCode: true`（仅调试用户代码）
3. 减少日志输出：
   ```python
   import logging
   logging.getLogger().setLevel(logging.WARNING)
   ```

4. 使用 Docker 卷缓存（在 `docker-compose.yml` 中）：
   ```yaml
   volumes:
     - .:/workspace:cached
   ```

### 问题 6: 符号文件未加载

**症状**: C/C++ 调试时无法查看变量值

**解决方案**:

1. 确保使用调试编译选项构建：
   ```bash
   CFLAGS="-g -O0" make runtime
   ```

2. 检查符号文件是否存在：
   ```bash
   file bin/plcopen_runtime
   # 应该显示: not stripped
   ```

3. 在 gdb 中手动加载符号：
   ```gdb
   symbol-file bin/plcopen_runtime
   ```

## 📚 进阶技巧

### 1. 使用 Docker Exec 直接调试

快速测试时，可以直接在容器中运行：

```bash
# Python
docker exec -it plcopen-dev python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py

# C with gdb
docker exec -it plcopen-dev gdb --args bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 2. 使用 Valgrind 检查内存问题

```bash
docker exec -it plcopen-dev valgrind --leak-check=full --show-leak-kinds=all bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 3. 使用 strace 追踪系统调用

```bash
docker exec -it plcopen-dev strace -f -e trace=all bin/plcopen_runtime --config config/pid_temperature.yaml
```

### 4. 远程调试真实设备

如果需要调试部署到真实设备（如树莓派）的代码：

1. 在设备上启动 debugpy：
   ```bash
   python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client /opt/plcopen/python/main.py
   ```

2. 使用配置：`Python: 附加到远程设备`
3. 修改 `host` 为设备 IP：`192.168.1.100`

## 📖 相关文档

- [VS Code Python 调试](https://code.visualstudio.com/docs/python/debugging)
- [VS Code C++ 调试](https://code.visualstudio.com/docs/cpp/cpp-debug)
- [debugpy 文档](https://github.com/microsoft/debugpy)
- [GDB 用户手册](https://sourceware.org/gdb/documentation/)
- [Docker Compose 文档](https://docs.docker.com/compose/)

## 🎯 总结

| 场景 | 推荐方法 | 命令 |
|------|---------|------|
| 快速开始 | 使用启动脚本 | `.\debug-container.ps1` 或 `./debug-container.sh` |
| Python 开发 | VS Code Dev Container | 在 VS Code 中重新打开 |
| C/C++ 开发 | gdbserver + VS Code | `gdbserver :9000 bin/plcopen_runtime` |
| 混合调试 | 复合调试配置 | 选择 "混合调试: Python + C++" |
| 跨平台测试 | ARM64 容器 | `docker-compose up -d arm-test` |

祝调试愉快！🐛✨
