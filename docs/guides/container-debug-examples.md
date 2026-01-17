# 容器调试示例

> ⚠️ **历史文档**: 本文档中的命令基于旧的 PowerShell 脚本。  
> **Bash 等效命令**: 使用 `./run.sh --shell` 代替 `debug-container.ps1`。  
> 详见 [MIGRATION_FROM_POWERSHELL.md](../../MIGRATION_FROM_POWERSHELL.md)

这里提供几个实际的调试示例，帮助你快速上手。

## 示例 1: 调试 Python PID 控制器

### 场景
调试 PID 温度控制示例，观察 PID 参数如何影响控制效果。

### 步骤

#### 1. 启动容器和调试服务器

**Windows**:
```powershell
.\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
```

**Linux/macOS**:
```bash
./debug-container.sh -e pid_temperature -w
```

#### 2. 在 VS Code 中设置断点

打开 [python/examples/pid_temperature.py](../python/examples/pid_temperature.py)，在以下位置设置断点：

- 第 50 行：PID 计算后
- 第 55 行：温度更新后

#### 3. 连接调试器

1. 按 `F5`
2. 选择 "Python: 附加到 Docker 容器 (x86_64)"
3. 等待断点触发

#### 4. 调试技巧

**查看 PID 内部状态**:
在调试控制台中执行：
```python
print(f"Kp={pid.kp}, Ki={pid.ki}, Kd={pid.kd}")
print(f"Error={error}, Integral={pid.integral}, Derivative={pid.derivative}")
```

**实时修改参数**:
```python
# 增大比例增益
pid.kp = 5.0

# 禁用积分项
pid.ki = 0.0
```

**条件断点**:
在断点上右键 → 编辑断点 → 添加条件：
```python
temperature > 80.0 or abs(error) > 20.0
```

## 示例 2: 调试 C 代码中的 PID 函数

### 场景
调试 C 语言实现的 PID 功能块，检查计算逻辑。

### 步骤

#### 1. 启动容器
```bash
docker-compose up -d dev
```

#### 2. 使用 gdbserver 启动程序
```bash
docker exec -it plcopen-dev gdbserver :9000 /workspace/bin/plcopen_runtime --config /workspace/config/pid_temperature.yaml
```

#### 3. 在 C 代码中设置断点

打开 [src/function_blocks/fb_pid.c](../src/function_blocks/fb_pid.c)，在 `pid_update` 函数中设置断点。

#### 4. 连接 gdb 调试器

1. 按 `F5`
2. 选择 "C++: 附加到容器中的进程 (gdbserver)"
3. 程序会在断点处暂停

#### 5. GDB 调试技巧

**查看 PID 结构体**:
```gdb
print *pid
print pid->kp
print pid->integral
```

**查看内存**:
```gdb
# 查看 PID 结构体的内存布局
x/20xw pid

# 以浮点数格式查看
x/10f &pid->kp
```

**监视变量**:
```gdb
# 当 integral 变化时暂停
watch pid->integral

# 当 output 被读取时暂停
rwatch pid->output
```

## 示例 3: 混合调试（Python + C）

### 场景
同时调试 Python 代码和底层的 C 扩展模块，追踪调用链。

### 步骤

#### 1. 启动两个调试服务器

**终端 1 - Python debugpy**:
```bash
docker exec -it plcopen-dev bash
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/basic_pid.py
```

**终端 2 - C gdbserver** (可选，如果需要调试 C 扩展):
```bash
docker exec -it plcopen-dev bash
# 使用 gdb 附加到 Python 进程
# (需要先知道 Python 进程的 PID)
```

#### 2. 设置断点

- **Python**: 在 `basic_pid.py` 中调用 `pid.compute()` 的位置
- **C**: 在 `py_pid.c` 中的 `py_pid_compute()` 函数

#### 3. 使用复合调试配置

1. 按 `F5`
2. 选择 "混合调试: Python + C++"
3. 两个调试器会同时启动

#### 4. 追踪调用链

单步执行时可以看到：
1. Python 代码调用 `pid.compute()`
2. 进入 C 扩展的 `py_pid_compute()`
3. 调用底层的 `pid_update()`
4. 返回到 Python

## 示例 4: 使用 Valgrind 检查内存泄漏

### 场景
检查运行时程序是否存在内存泄漏。

### 步骤

#### 1. 运行 VS Code 任务

1. 按 `Ctrl+Shift+P`
2. 选择 "Tasks: Run Task"
3. 选择 "容器: 运行内存检查 (Valgrind)"

或者直接在容器中运行：

```bash
docker exec plcopen-dev valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --verbose \
  /workspace/bin/plcopen_runtime --config /workspace/config/pid_temperature.yaml
```

#### 2. 分析输出

查看 Valgrind 报告：

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: 1,234 allocs, 1,234 frees, 12,345 bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

如果有内存泄漏：

```
==12345== 100 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2FB0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x400536: pid_create (fb_pid.c:15)
==12345==    by 0x400550: main (main.c:20)
```

## 示例 5: 调试脚本加载问题

### 场景
Python 脚本无法加载或执行时的调试。

### 步骤

#### 1. 启用详细日志

修改 [src/runtime/py_embed.c](../src/runtime/py_embed.c) 或使用环境变量：

```bash
docker exec -it plcopen-dev bash
export PYTHONVERBOSE=1
python3 python/examples/pid_temperature.py
```

#### 2. 检查 Python 路径

在调试控制台中：

```python
import sys
print(sys.path)

# 检查模块是否可以导入
import plcopen
print(plcopen.__file__)
```

#### 3. 使用 strace 追踪文件访问

```bash
docker exec plcopen-dev strace -e trace=open,openat,stat python3 python/examples/pid_temperature.py 2>&1 | grep -E '(plcopen|\.py)'
```

## 示例 6: 性能分析

### 场景
分析控制循环的性能，找出瓶颈。

### 步骤

#### 1. 使用 Python cProfile

```python
import cProfile
import pstats

# 在 pid_temperature.py 中添加
profiler = cProfile.Profile()
profiler.enable()

# 运行控制循环
for i in range(1000):
    output = pid.compute(SP=setpoint, PV=temperature)
    # ...

profiler.disable()
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(20)  # 显示前 20 个最耗时的函数
```

#### 2. 使用 time 命令测量

```bash
docker exec plcopen-dev bash -c "time python3 python/examples/pid_temperature.py"
```

#### 3. 使用 perf 工具（Linux）

```bash
docker exec plcopen-dev perf record -g python3 python/examples/pid_temperature.py
docker exec plcopen-dev perf report
```

## 示例 7: 远程设备调试

### 场景
调试部署到树莓派或其他嵌入式设备上的代码。

### 步骤

#### 1. 在设备上安装 debugpy

```bash
# SSH 到设备
ssh pi@192.168.1.100

# 安装 debugpy
pip3 install debugpy
```

#### 2. 在设备上启动调试服务器

```bash
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client /opt/plcopen/python/main.py
```

#### 3. 在 VS Code 中连接

1. 修改 `.vscode/launch.json` 中的 "Python: 附加到远程设备" 配置
2. 将 `host` 改为设备 IP：`192.168.1.100`
3. 按 `F5` 连接

#### 4. 注意事项

- 确保设备防火墙允许 5678 端口
- 确保路径映射正确（设备上的路径可能不同）
- 网络延迟可能导致调试响应较慢

## 常见调试任务快速参考

| 任务 | 命令 |
|------|------|
| 启动 Python 调试 | `docker exec -it plcopen-dev python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py` |
| 启动 C 调试 | `docker exec -it plcopen-dev gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml` |
| 查看容器日志 | `docker logs -f plcopen-dev` |
| 进入容器 Shell | `docker exec -it plcopen-dev bash` |
| 重启容器 | `docker-compose restart dev` |
| 停止容器 | `docker-compose down` |
| 查看 Python 环境 | `docker exec plcopen-dev python3 -m pip list` |
| 运行单元测试 | `docker exec plcopen-dev pytest tests/unit/ -v` |
| 检查内存泄漏 | `docker exec plcopen-dev valgrind --leak-check=full bin/plcopen_runtime` |

## 调试技巧总结

### Python 调试
- ✅ 使用 `justMyCode: false` 调试第三方库
- ✅ 使用条件断点减少中断次数
- ✅ 使用日志断点避免修改代码
- ✅ 在调试控制台中直接执行代码

### C/C++ 调试
- ✅ 编译时使用 `-g -O0` 获得最佳调试体验
- ✅ 使用 `watch` 命令监视变量变化
- ✅ 使用 `x` 命令查看内存内容
- ✅ 使用 `bt` 命令查看调用栈

### 性能调试
- ✅ 使用 Valgrind 检查内存问题
- ✅ 使用 cProfile 分析 Python 性能
- ✅ 使用 strace 追踪系统调用
- ✅ 使用 time 命令快速测量执行时间

## 进一步学习

- [Python debugpy 文档](https://github.com/microsoft/debugpy/wiki)
- [GDB 用户手册](https://sourceware.org/gdb/documentation/)
- [Valgrind 快速入门](https://valgrind.org/docs/manual/quick-start.html)
- [VS Code 调试技巧](https://code.visualstudio.com/docs/editor/debugging)

---

💡 **提示**: 这些示例可以组合使用，根据实际调试需求灵活选择。
