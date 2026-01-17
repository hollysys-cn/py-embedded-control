# PLCopen Python 运行时 - 远程调试快速入门

本文档提供了完整的远程调试设置和使用指南，帮助您快速开始调试 Python 控制脚本。

## 概述

本项目支持使用 debugpy 进行远程 Python 调试，允许您从 VS Code 连接到运行在 Docker 容器中的 Python 脚本，设置断点、单步执行和查看变量。

## 前提条件

- Docker 和 Docker Compose 已安装
- VS Code 已安装
- Python 扩展已安装（ms-python.python）

## 快速开始（3 步）

### 步骤 1：启动 Docker 容器

```bash
# 构建并启动开发容器
docker-compose up -d --build dev

# 验证容器运行状态
docker ps
```

您应该看到名为 `plcopen-dev` 的容器正在运行，端口 5678（debugpy）和 9000（gdbserver）已映射。

### 步骤 2：在容器中启动调试服务器

有两种方式启动调试服务器：

#### 方式 A：使用 VS Code 任务（推荐）

1. 按 `Ctrl+Shift+P` 打开命令面板
2. 选择 `Tasks: Run Task`
3. 选择 `容器: 启动 Python 调试服务器`

#### 方式 B：使用命令行

```bash
# 启动调试服务器并等待客户端连接
docker exec plcopen-dev bash -c "cd /workspace && python3 -Xfrozen_modules=off -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py"
```

**注意**：`-Xfrozen_modules=off` 参数用于禁用冻结模块，避免调试器错过断点。

### 步骤 3：附加 VS Code 调试器

1. 切换到 VS Code 的"运行和调试"视图（`Ctrl+Shift+D`）
2. 从下拉菜单中选择 `Python: 附加到 Docker 容器 (x86_64)`
3. 点击绿色的"开始调试"按钮（F5）
4. VS Code 将连接到容器中的调试服务器

现在您可以：
- 在 `python/examples/pid_temperature.py` 中设置断点
- 使用 F10/F11 单步执行代码
- 在"变量"面板查看实时数据
- 在"调试控制台"中评估表达式

## 调试配置详解

### launch.json 配置

项目已预先配置了多个调试配置（`.vscode/launch.json`）：

#### 1. Python: 附加到 Docker 容器 (x86_64)
- **用途**：连接到 x86_64 开发容器
- **端口**：5678
- **路径映射**：`${workspaceFolder}` → `/workspace`

#### 2. Python: 附加到 Docker 容器 (ARM64)
- **用途**：连接到 ARM64 测试容器
- **端口**：5679
- **路径映射**：`${workspaceFolder}` → `/workspace`

#### 3. Python: 启动 PID 温度控制示例
- **用途**：直接在本地启动示例（不使用 Docker）
- **环境**：自动设置 PYTHONPATH

### 配置文件

调试配置位于 `config/pid_temperature_debug.yaml`：

```yaml
# 调试配置
debug:
  enabled: true        # 启用调试
  host: "0.0.0.0"      # 监听所有网络接口
  port: 5678           # debugpy 端口
  timeout: 10          # 连接超时（秒）
```

## 调试工作流程

### 场景 1：调试 PID 温度控制示例

1. **设置断点**：
   - 打开 `python/examples/pid_temperature.py`
   - 在 `step()` 函数中点击行号左侧设置断点

2. **启动调试**：
   - 按照"快速开始"步骤启动容器和调试服务器
   - 附加 VS Code 调试器

3. **观察控制过程**：
   - 程序将在断点处暂停
   - 查看 `temperature` 和 `pid` 变量的当前值
   - 使用 F10 单步执行，观察 PID 计算过程

4. **评估表达式**：
   - 在"调试控制台"中输入：
     ```python
     pid.get_params()
     temperature
     setpoint
     ```

### 场景 2：修改运行时参数

在调试控制台中动态修改 PID 参数：

```python
# 增加比例增益
pid.set_params(Kp=5.0)

# 查看新参数
pid.get_params()
```

### 场景 3：调试自定义控制逻辑

1. 复制 `python/examples/pid_temperature.py` 创建新脚本
2. 修改 `config/pid_temperature_debug.yaml` 中的 `python_script` 路径
3. 按照相同流程启动调试

## 常见问题

### Q1：调试器无法连接

**问题**：VS Code 显示"无法连接到调试服务器"

**解决方案**：
1. 检查容器是否运行：`docker ps`
2. 检查端口映射：`docker port plcopen-dev`
3. 检查调试服务器进程：`docker exec plcopen-dev ps aux | grep debugpy`
4. 查看容器日志：`docker logs plcopen-dev`

### Q2：断点不生效

**问题**：设置的断点显示为灰色或被跳过

**解决方案**：
1. 确保使用 `-Xfrozen_modules=off` 参数启动 Python
2. 检查路径映射是否正确（`${workspaceFolder}` → `/workspace`）
3. 确保文件已保存（`Ctrl+S`）
4. 重新启动调试会话

### Q3：调试会话中断

**问题**：调试过程中突然断开连接

**解决方案**：
1. 检查网络连接
2. 增加调试超时时间（在 launch.json 中添加 `"timeout": 30000`）
3. 检查 Python 脚本是否有未捕获的异常

### Q4：变量显示不完整

**问题**：在"变量"面板中看不到某些变量

**解决方案**：
1. 确保 `justMyCode` 设置为 `false`（已默认配置）
2. 在"调试控制台"中使用 `locals()` 查看所有局部变量
3. 使用 `globals()` 查看全局变量

## 高级技巧

### 1. 使用条件断点

右键点击断点，选择"编辑断点"，设置条件：

```python
temperature > 30.0
```

断点仅在温度超过 30°C 时触发。

### 2. 使用日志点

不中断程序执行，只记录信息：

```python
temperature={temperature}, output={output}
```

### 3. 监视表达式

在"监视"面板添加表达式实时查看：

```python
pid.get_params()
temperature - setpoint  # 当前误差
```

### 4. 调用堆栈分析

在"调用堆栈"面板查看函数调用链，了解程序执行路径。

## VS Code 任务参考

项目提供了多个预定义任务（`.vscode/tasks.json`）：

| 任务名称 | 描述 | 用途 |
|---------|------|------|
| Docker: 启动开发容器 | `docker-compose up -d dev` | 启动 x86_64 容器 |
| Docker: 停止开发容器 | `docker-compose down dev` | 停止容器 |
| Docker: 重启开发容器 | `docker-compose restart dev` | 重启容器 |
| 容器: 启动 Python 调试服务器 | 在容器中启动 debugpy | 准备调试环境 |
| 容器: 运行 PID 示例 | 直接运行示例（无调试） | 快速测试 |
| 容器: 构建项目 | 编译 C 扩展和运行时 | 重新构建代码 |

使用方式：`Ctrl+Shift+P` → `Tasks: Run Task` → 选择任务

## 性能考虑

- **调试模式性能**：debugpy 会降低约 20-30% 的性能
- **生产部署**：在生产环境中禁用调试（`debug.enabled: false`）
- **网络延迟**：远程调试受网络延迟影响，局域网延迟通常 <10ms

## 安全注意事项

- **仅开发环境**：不要在生产环境启用 debugpy
- **防火墙配置**：限制 5678 端口仅允许信任的 IP 访问
- **Docker 网络**：默认配置仅映射到 localhost，不暴露到公网

## 下一步

- 阅读 [docs/debugging.md](docs/debugging.md) 了解 C 代码调试（gdbserver）
- 查看 [docs/user_guide.md](docs/user_guide.md) 了解完整功能
- 参考 [docs/api_reference.md](docs/api_reference.md) 了解所有 API

## 支持

遇到问题？

1. 查看 [docs/faq.md](docs/faq.md)
2. 检查容器日志：`docker logs plcopen-dev`
3. 启用调试日志：在 launch.json 中设置 `"logToFile": true`

---

**最后更新**：2026-01-17
**适用版本**：PLCopen Python Runtime v1.0.0
