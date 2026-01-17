# PLCopen Python 运行时环境 - 常见问题

## 目录

1. [安装问题](#安装问题)
2. [编译问题](#编译问题)
3. [运行时问题](#运行时问题)
4. [性能问题](#性能问题)
5. [PID 调优](#pid-调优)
6. [Python 脚本问题](#python-脚本问题)
7. [Docker 问题](#docker-问题)
8. [调试技巧](#调试技巧)

---

## 安装问题

### Q: 安装时提示 "找不到 Python.h"

**A:** 需要安装 Python 开发包：

```bash
# Debian/Ubuntu
sudo apt-get install python3-dev

# CentOS/RHEL
sudo yum install python3-devel

# Alpine Linux
apk add python3-dev

# macOS
brew install python3
```

### Q: 安装时提示 "找不到 pthread 库"

**A:** pthread 通常是系统自带的，但可以尝试：

```bash
# Debian/Ubuntu
sudo apt-get install libc6-dev

# CentOS/RHEL
sudo yum install glibc-devel
```

### Q: Make 构建失败，提示 "no rule to make target"

**A:** 确保在项目根目录执行 make：

```bash
cd /path/to/py-embedded-control
make build
```

---

## 编译问题

### Q: 编译时提示 "implicit declaration of function clock_nanosleep"

**A:** 需要定义 `_POSIX_C_SOURCE`，已在 Makefile 和 setup.py 中配置：

```makefile
CFLAGS += -D_POSIX_C_SOURCE=200809L
```

### Q: 链接时提示 "undefined reference to Python symbols"

**A:** 确保正确链接 Python 库：

```makefile
LDFLAGS += -lpython3.13 $(PYTHON_LIBS)
```

检查 Python 版本：

```bash
python3 --version
python3-config --ldflags
```

### Q: GCC 版本过旧，不支持 C11

**A:** 升级 GCC 到 7.0 或更高版本：

```bash
# Debian/Ubuntu
sudo apt-get install gcc-9

# 设置默认版本
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 100
```

---

## 运行时问题

### Q: 运行时提示 "无法加载配置文件"

**A:** 检查以下事项：

1. 配置文件路径是否正确
2. 配置文件格式是否正确（YAML）
3. 文件权限是否可读

```bash
# 测试配置文件
cat config/pid_temperature.yaml

# 检查权限
ls -l config/pid_temperature.yaml
```

### Q: 运行时提示 "无法加载 Python 脚本"

**A:** 检查脚本路径和语法：

1. 确认脚本路径正确
2. 检查脚本语法：

```bash
python3 -m py_compile python/examples/my_script.py
```

3. 确认脚本包含 `init()` 和 `step()` 函数

### Q: 运行时提示 "调度器休眠被中断"

**A:** 这是正常现象，系统信号（如 Ctrl+C）会中断休眠。如果频繁出现：

1. 检查系统负载
2. 检查是否有其他进程发送信号
3. 考虑使用 Docker 隔离环境

### Q: 如何优雅关闭运行时？

**A:** 使用 Ctrl+C 或发送 SIGTERM 信号：

```bash
# Ctrl+C
^C

# 或发送信号
kill -TERM <pid>
```

运行时会自动清理资源并退出。

---

## 性能问题

### Q: 周期时间不稳定，经常抖动

**A:** 调优建议：

1. **减少计算量**：优化 `step()` 函数
2. **增加周期时间**：设置更长的 `cycle_time_ms`
3. **使用 CPU 亲和性**：绑定到特定 CPU 核心

```bash
# 绑定到 CPU 0
taskset -c 0 bin/plcopen_runtime --config config/my_config.yaml
```

4. **禁用不必要的服务**：减少系统负载

### Q: 周期时间经常超时

**A:** 检查和优化：

1. **Profile 脚本**：找出耗时操作

```python
import time

def step():
    start = time.time()
    # 控制逻辑
    elapsed = time.time() - start
    print(f"step() 耗时: {elapsed*1000:.1f} ms")
```

2. **避免阻塞操作**：不要在 `step()` 中执行：
   - 文件 I/O
   - 网络请求
   - 复杂计算

3. **增加超时阈值**：

```yaml
scheduler:
  timeout_ms: 80  # 增加到周期时间的 80%
```

### Q: CPU 使用率过高

**A:** 优化建议：

1. **检查死循环**：确认脚本没有死循环
2. **增加周期时间**：降低调用频率
3. **优化算法**：减少计算复杂度
4. **使用 C 扩展**：将耗时逻辑移到 C 层

---

## PID 调优

### Q: PID 输出振荡，无法稳定

**A:** 调优步骤：

1. **降低 Kp**：减小比例增益
2. **增大 Kd**：增加微分增益抑制振荡
3. **检查采样周期**：确保周期时间合适

```python
# 振荡示例：Kp 过大
pid = PID(Kp=10.0, Ki=1.0, Kd=0.0)  # 振荡

# 改进：降低 Kp，增加 Kd
pid = PID(Kp=3.0, Ki=0.5, Kd=0.5)  # 稳定
```

### Q: PID 响应过慢

**A:** 调优步骤：

1. **增大 Kp**：加快响应速度
2. **增大 Ki**：消除稳态误差
3. **检查执行器**：确认输出范围合适

```python
# 慢响应示例：Kp 过小
pid = PID(Kp=0.5, Ki=0.1, Kd=0.0)  # 慢

# 改进：增大 Kp 和 Ki
pid = PID(Kp=3.0, Ki=0.5, Kd=0.1)  # 快
```

### Q: PID 存在稳态误差

**A:** 调优步骤：

1. **增大 Ki**：增加积分增益
2. **检查输出饱和**：确认输出没有达到限制
3. **检查外部干扰**：排查系统扰动

```python
# 存在稳态误差：Ki = 0
pid = PID(Kp=2.0, Ki=0.0, Kd=0.1)  # 有误差

# 改进：增加 Ki
pid = PID(Kp=2.0, Ki=0.5, Kd=0.1)  # 无误差
```

### Q: 如何使用 Ziegler-Nichols 方法调优？

**A:** 步骤：

1. **设置 Ki=0, Kd=0**
2. **增大 Kp** 直到系统持续振荡
3. **记录临界增益 Ku** 和振荡周期 Tu
4. **计算 PID 参数**：

```python
# Ziegler-Nichols 经典 PID
Kp = 0.6 * Ku
Ki = 1.2 * Ku / Tu
Kd = 0.075 * Ku * Tu

pid = PID(Kp=Kp, Ki=Ki, Kd=Kd)
```

---

## Python 脚本问题

### Q: 脚本中如何导入其他模块？

**A:** 直接使用标准 Python 导入：

```python
import math
import sys
from plcopen import PID

def step():
    value = math.sin(time)
```

### Q: 如何在脚本中访问文件？

**A:** 使用相对或绝对路径：

```python
def init():
    with open('data/config.txt', 'r') as f:
        config = f.read()
```

### Q: 脚本中如何使用第三方库？

**A:** 安装后直接导入：

```bash
pip3 install numpy

# 在脚本中
import numpy as np

def step():
    data = np.array([1, 2, 3])
```

### Q: 如何在脚本中记录日志？

**A:** 使用 Python logging 模块：

```python
import logging

def init():
    logging.basicConfig(level=logging.INFO)

def step():
    logging.info("温度: 25.0°C")
```

---

## Docker 问题

### Q: Docker 构建失败，提示网络错误

**A:** 使用国内镜像源：

```dockerfile
# 在 Dockerfile 中添加
RUN sed -i 's/deb.debian.org/mirrors.aliyun.com/g' /etc/apt/sources.list
```

### Q: Docker 容器中如何访问主机文件？

**A:** 使用卷挂载：

```bash
docker run -v /host/path:/container/path plcopen-runtime
```

### Q: Docker 容器中如何调试？

**A:** 使用交互模式：

```bash
docker run -it plcopen-runtime /bin/bash
```

### Q: 如何缩小 Docker 镜像大小？

**A:** 优化建议：

1. **使用 Alpine Linux**（体积小）
2. **多阶段构建**（分离构建和运行环境）
3. **清理缓存**：

```dockerfile
RUN apt-get update && apt-get install -y ... \
    && rm -rf /var/lib/apt/lists/*
```

---

## 调试技巧

### Q: 如何启用详细日志？

**A:** 设置日志级别为 DEBUG：

```yaml
logging:
  level: DEBUG
```

### Q: 如何测量 Python 函数执行时间？

**A:** 使用 time 模块：

```python
import time

def step():
    start = time.time()
    # 控制逻辑
    elapsed = (time.time() - start) * 1000
    print(f"执行时间: {elapsed:.2f} ms")
```

### Q: 如何在脚本中设置断点？

**A:** 使用 pdb：

```python
import pdb

def step():
    pdb.set_trace()  # 断点
    # 控制逻辑
```

### Q: 如何查看调度器统计？

**A:** 运行时会定期输出统计信息：

```
调度器统计：周期数=1000, 平均=98.5 ms, 最小=97.2 ms, 最大=102.3 ms, 超时=5
```

### Q: 如何模拟传感器数据？

**A:** 在脚本中生成模拟数据：

```python
import random
import math

def step():
    # 正弦波 + 噪声
    t = time.time()
    sensor_value = 25.0 + 5.0 * math.sin(t) + random.gauss(0, 0.5)
```

### Q: 如何记录数据到文件？

**A:** 使用 Python 文件操作：

```python
import csv

csv_file = None

def init():
    global csv_file
    csv_file = open('data/log.csv', 'w', newline='')
    writer = csv.writer(csv_file)
    writer.writerow(['时间', '温度', '输出'])

def step():
    writer = csv.writer(csv_file)
    writer.writerow([time.time(), temperature, output])
```

---

## 更多帮助

如果以上问题没有解决您的问题，请：

1. 查看 [用户手册](user_guide.md)
2. 查看 [API 参考](api_reference.md)
3. 查看 [示例文档](examples/pid_temperature.md)
4. 在 GitHub 提交 Issue: https://github.com/hollysys-cn/py-embedded-control/issues

---

**最后更新:** 2026-01-01
