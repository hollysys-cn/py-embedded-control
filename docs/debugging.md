# 远程调试 Python 脚本

本文档说明如何使用 VS Code 远程调试运行在 PLCopen 运行时环境中的 Python 控制脚本。

## 概述

PLCopen 运行时集成了 [debugpy](https://github.com/microsoft/debugpy)，允许你使用 VS Code 或其他 DAP（Debug Adapter Protocol）兼容的调试器远程调试 Python 脚本。

**关键特性**：
- ✅ 断点设置：在 Python 脚本的任意行设置断点
- ✅ 变量查看：实时查看变量值（局部变量、全局变量、功能块状态）
- ✅ 单步执行：逐行执行代码，观察控制逻辑流程
- ✅ 表达式求值：在调试控制台中评估 Python 表达式
- ✅ 非侵入式：调试功能不影响控制逻辑的正常运行

## 前提条件

### 1. 安装 debugpy

在运行时环境中安装 debugpy：

```bash
# 使用 pip 安装
pip install debugpy

# 或者使用项目的 requirements.txt
pip install -r requirements.txt
```

### 2. 启用调试配置

在配置文件中启用调试（例如 `config/pid_temperature_debug.yaml`）：

```yaml
debug:
  enabled: true        # 启用调试
  host: "0.0.0.0"      # 监听地址（0.0.0.0 允许远程连接）
  port: 5678           # debugpy 默认端口
  timeout: 10          # 连接超时（秒）
```

**配置说明**：
- **enabled**: 设置为 `true` 启用调试功能
- **host**:
  - `0.0.0.0` - 监听所有网络接口（允许远程连接）
  - `127.0.0.1` - 仅允许本地连接
- **port**: debugpy 服务器监听端口（默认 5678）
- **timeout**: 连接超时时间，超时后程序继续运行（不阻塞控制逻辑）

## VS Code 调试配置

### 1. 创建 launch.json

在项目根目录创建 `.vscode/launch.json` 文件：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: 附加到运行时",
            "type": "debugpy",
            "request": "attach",
            "connect": {
                "host": "localhost",
                "port": 5678
            },
            "pathMappings": [
                {
                    "localRoot": "${workspaceFolder}/python",
                    "remoteRoot": "${workspaceFolder}/python"
                }
            ],
            "justMyCode": false
        }
    ]
}
```

**配置说明**：
- **name**: 调试配置名称
- **type**: 必须是 `debugpy`（VS Code 需要安装 Python 扩展）
- **request**: `attach` 表示附加到已运行的进程
- **connect.host**: 运行时主机地址
  - `localhost` - 本地调试
  - 远程 IP - 远程调试（例如 Docker 容器或嵌入式设备）
- **connect.port**: 必须与配置文件中的 `debug.port` 一致
- **pathMappings**: 路径映射（本地路径 → 远程路径）
- **justMyCode**: `false` 允许调试第三方库代码

### 2. Docker 环境配置

如果运行时在 Docker 容器中，需要映射调试端口：

```yaml
# docker-compose.yml
services:
  runtime:
    build: .
    ports:
      - "5678:5678"  # 映射调试端口
    volumes:
      - .:/workspace
```

VS Code 配置：

```json
{
    "name": "Python: 附加到 Docker 容器",
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
    ]
}
```

## 调试步骤

### 1. 启动运行时

使用调试配置启动运行时：

```bash
./bin/plcopen_runtime --config config/pid_temperature_debug.yaml
```

运行时输出示例：

```
[INFO] 启动 debugpy 服务器: 0.0.0.0:5678
[INFO] debugpy 服务器已启动，等待调试器连接...
[INFO] VS Code 配置: "host": "0.0.0.0", "port": 5678
[INFO] 控制循环启动，周期: 100ms
```

### 2. 在 VS Code 中设置断点

1. 打开 Python 脚本文件（例如 `python/examples/pid_temperature_control.py`）
2. 点击行号左侧，设置断点（红点）
3. 常见断点位置：
   - `init()` 函数：查看初始化逻辑
   - `step()` 函数：观察每个控制周期的执行

### 3. 启动调试会话

1. 在 VS Code 中按 `F5` 或点击"运行 → 启动调试"
2. 选择配置："Python: 附加到运行时"
3. VS Code 连接到 debugpy 服务器

连接成功后：
- 状态栏显示调试工具栏
- 断点变为实心红点（已激活）
- 程序在断点处暂停

### 4. 调试操作

**调试工具栏**：
- **继续 (F5)**: 继续执行直到下一个断点
- **单步跳过 (F10)**: 执行当前行，不进入函数内部
- **单步进入 (F11)**: 进入函数内部
- **单步跳出 (Shift+F11)**: 跳出当前函数
- **重启 (Ctrl+Shift+F5)**: 重新启动调试会话
- **停止 (Shift+F5)**: 停止调试

**查看变量**：
- **变量面板**: 自动显示当前作用域的所有变量
- **监视表达式**: 添加自定义表达式（例如 `pid.kp`）
- **调用堆栈**: 查看函数调用链

**调试控制台**：
- 评估 Python 表达式：`>>> print(setpoint)`
- 修改变量值：`>>> output = 50.0`（仅当前会话有效）

## 调试示例

### 示例 1: 调试 PID 控制器

1. **设置断点**：在 `step()` 函数的 PID 计算处

```python
def step():
    # 获取当前温度
    current_temp = fb.get("temperature_pid", "process_value")

    # <--- 在此处设置断点

    # 执行 PID 计算
    output = fb.compute("temperature_pid", current_temp)
```

2. **启动调试**：程序在断点处暂停

3. **查看变量**：
   - `current_temp`: 当前温度值
   - `output`: PID 输出值
   - `fb.state`: 功能块内部状态

4. **单步执行**：观察 PID 计算过程

5. **修改变量**（调试控制台）：
   ```python
   >>> current_temp = 100.0  # 模拟高温场景
   >>> output = fb.compute("temperature_pid", current_temp)
   >>> print(output)
   ```

### 示例 2: 调试初始化逻辑

1. **设置断点**：在 `init()` 函数

```python
def init():
    print("初始化温度控制")
    fb.set("temperature_pid", "setpoint", 75.0)  # <--- 断点
```

2. **查看功能块配置**：
   - 检查 PID 参数（kp, ki, kd）
   - 验证目标值设置

3. **条件断点**：仅在特定条件下暂停
   - 右键断点 → "编辑断点" → "条件"
   - 输入条件：`setpoint > 80.0`

## 常见问题

### 1. 连接超时

**问题**：VS Code 无法连接到 debugpy 服务器

**解决方法**：
- 检查运行时是否已启动：`ps aux | grep plcopen_runtime`
- 验证调试端口是否监听：`netstat -tuln | grep 5678`
- 检查防火墙设置（允许 5678 端口）
- Docker 环境：确保端口已映射（`docker ps`）

### 2. 断点未激活

**问题**：断点显示为灰色圆圈（未激活）

**原因**：
- 路径映射不正确
- Python 脚本未被运行时加载

**解决方法**：
- 检查 `launch.json` 中的 `pathMappings`
- 确保 `localRoot` 和 `remoteRoot` 路径正确
- 使用绝对路径或 `${workspaceFolder}` 变量

### 3. 调试影响实时性

**问题**：断点暂停导致控制周期超时

**说明**：这是预期行为。调试时，程序暂停在断点处，控制周期无法按时完成。

**建议**：
- 调试时使用较长的周期（例如 1000ms）
- 使用条件断点减少暂停次数
- 使用日志而非断点进行生产环境调试

### 4. debugpy 未安装

**问题**：运行时输出"无法导入 debugpy 模块"

**解决方法**：
```bash
pip install debugpy
```

或者禁用调试功能：
```yaml
debug:
  enabled: false
```

### 5. 远程调试连接失败

**问题**：远程主机无法连接（Docker 或嵌入式设备）

**解决方法**：
- 使用 `0.0.0.0` 而非 `127.0.0.1`（监听所有接口）
- Docker：映射端口到主机（`-p 5678:5678`）
- 嵌入式设备：检查网络连通性（`ping <device_ip>`）

## 高级技巧

### 1. 条件断点

仅在特定条件下暂停（减少干扰）：

```python
# 右键断点 → "编辑断点" → "条件"
# 条件表达式示例：
current_temp > 90.0           # 温度过高时暂停
output < 0 or output > 100    # 输出异常时暂停
cycle_count % 100 == 0        # 每 100 个周期暂停一次
```

### 2. 日志断点

不暂停程序，仅打印日志：

```python
# 右键断点 → "编辑断点" → "日志消息"
# 消息模板：
"温度: {current_temp}, 输出: {output}"
```

### 3. 监视功能块状态

在"监视"面板添加表达式：

```python
fb.get("temperature_pid", "process_value")
fb.get("temperature_pid", "error")
fb.get("temperature_pid", "integral")
```

### 4. 调试多个脚本

如果运行时加载多个 Python 脚本：

```json
{
    "pathMappings": [
        {
            "localRoot": "${workspaceFolder}/python/examples",
            "remoteRoot": "/workspace/python/examples"
        },
        {
            "localRoot": "${workspaceFolder}/python/plcopen",
            "remoteRoot": "/workspace/python/plcopen"
        }
    ]
}
```

## 性能影响

**调试模式的性能开销**：
- debugpy 服务器启动：+50ms（一次性）
- 未连接调试器：可忽略（<1% CPU）
- 调试器已连接：轻微（+2-5% CPU）
- 断点暂停：周期超时（预期行为）

**生产环境建议**：
- 禁用调试功能（`debug.enabled: false`）
- 使用日志系统进行诊断
- 仅在开发/测试环境启用调试

## 参考资料

- [debugpy 官方文档](https://github.com/microsoft/debugpy)
- [VS Code Python 调试指南](https://code.visualstudio.com/docs/python/debugging)
- [DAP (Debug Adapter Protocol)](https://microsoft.github.io/debug-adapter-protocol/)
