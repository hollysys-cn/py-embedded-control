# 快速入门指南：PLCopen Python 运行时

**版本**: 1.0.0
**目标用户**: 嵌入式系统开发工程师
**预计时间**: 30 分钟

## 概述

本指南将带你快速上手 PLCopen Python 运行时，完成一个 PID 温度控制示例项目。你将学会：

1. 安装和配置运行时环境
2. 编写第一个 PID 控制脚本
3. 运行和监控控制循环
4. 使用远程调试工具

---

## 先决条件

### 硬件要求
- **处理器**: x86_64 或 ARM Cortex-A7+
- **内存**: 最低 128MB RAM
- **操作系统**: Ubuntu 20.04+ / Debian 10+

### 软件要求
- Python 3.8 或更高版本
- GCC 编译器（支持 C11 标准）
- Git（可选）

### 确认环境
```bash
python3 --version   # 应显示 >= 3.8
gcc --version       # 应显示支持 -std=c11
```

---

## 第 1 步：安装运行时

### 方式 A：从源码构建（开发人员）

```bash
# 1. 克隆仓库
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control

# 2. 编译 C 扩展
cd src/python_bindings
make

# 3. 安装 Python 包
cd ../../python
pip3 install -e .

# 4. 验证安装
python3 -c "import plcopen; print(plcopen.__version__)"
```

### 方式 B：使用预编译包（生产环境）

```bash
# 下载 .whl 文件
wget https://releases.hollysys-cn.com/plcopen-runtime-1.0.0-py3-none-linux_x86_64.whl

# 安装
pip3 install plcopen-runtime-1.0.0-py3-none-linux_x86_64.whl

# 验证
python3 -c "import plcopen; print('安装成功')"
```

---

## 第 2 步：创建配置文件

创建 `config.yaml` 文件，定义运行时参数：

```yaml
# config.yaml - 运行时配置
runtime:
  # 控制循环周期（毫秒）
  cycle_time: 100

  # 用户脚本路径
  script_path: "control_script.py"

  # 日志配置
  logging:
    level: "INFO"            # DEBUG, INFO, WARNING, ERROR
    file: "runtime.log"
    max_size_mb: 10

  # 性能配置
  performance:
    max_cycle_jitter_ms: 5   # 最大周期抖动（毫秒）
    cpu_affinity: 0          # 绑定到 CPU 核心（-1 为不绑定）

# 远程调试配置（可选）
debug:
  enabled: false
  host: "0.0.0.0"
  port: 5678
```

**关键参数说明**:
- `cycle_time`: 控制循环执行周期，建议范围 10-1000ms
- `script_path`: 你的控制逻辑脚本（下一步创建）
- `cpu_affinity`: 用于实时性能优化，将进程绑定到特定 CPU 核心

---

## 第 3 步：编写控制脚本

创建 `control_script.py`，实现 PID 温度控制逻辑：

```python
"""
PID 温度控制脚本
目标：将温度稳定在 25°C
"""
import plcopen

# === 全局变量 ===
pid_controller = None
target_temperature = 25.0
current_temperature = 20.0  # 初始温度
cycle_count = 0

def init():
    """
    初始化函数：运行时启动时调用一次
    用于创建功能块实例和初始化变量
    """
    global pid_controller

    # 创建 PID 控制器
    pid_controller = plcopen.PID(
        Kp=2.0,              # 比例系数
        Ki=0.5,              # 积分系数
        Kd=0.1,              # 微分系数
        output_min=0.0,      # 加热器最小功率（关闭）
        output_max=100.0     # 加热器最大功率（全开）
    )

    print(f"[INIT] PID 控制器已初始化")
    print(f"[INIT] 目标温度: {target_temperature}°C")
    print(f"[INIT] PID 参数: {pid_controller.get_params()}")

def step():
    """
    周期执行函数：每个控制周期调用一次
    实现实际的控制逻辑
    """
    global current_temperature, cycle_count
    cycle_count += 1

    # --- 第 1 步：读取传感器 ---
    # 实际应用中替换为真实传感器读取，例如：
    # current_temperature = read_ds18b20_sensor()

    # 这里使用模拟值（简化的物理模型）
    import random
    current_temperature += simulate_temperature_change()

    # --- 第 2 步：PID 计算 ---
    control_output = pid_controller.compute(
        SP=target_temperature,      # 设定值
        PV=current_temperature      # 当前值
    )

    # --- 第 3 步：输出到执行器 ---
    # 实际应用中替换为 GPIO/PWM 输出，例如：
    # set_gpio_pwm(pin=18, duty_cycle=control_output)

    # 这里使用打印模拟
    if cycle_count % 10 == 0:  # 每 10 个周期输出一次
        error = target_temperature - current_temperature
        print(f"[周期 {cycle_count:04d}] "
              f"温度: {current_temperature:6.2f}°C | "
              f"误差: {error:+6.2f}°C | "
              f"控制: {control_output:5.1f}%")

def simulate_temperature_change():
    """
    模拟温度变化（简化物理模型）
    实际应用中删除此函数
    """
    import random

    # 获取上次控制输出
    params = pid_controller.get_params()
    state = pid_controller.get_state()

    # 加热效果：基于控制输出（假设线性）
    control_output = min(params["output_max"],
                        max(params["output_min"], state["last_error"] * 2.0))
    heating = control_output * 0.01

    # 冷却效果：自然散热（与环境温度差成正比）
    cooling = (current_temperature - 20.0) * 0.05

    # 测量噪声
    noise = random.uniform(-0.1, 0.1)

    return heating - cooling + noise
```

**脚本结构说明**:
1. **`init()` 函数**: 必须实现，用于初始化（创建功能块、设置变量）
2. **`step()` 函数**: 必须实现，每个控制周期执行
3. **全局变量**: 在模块作用域定义，用于在 `init()` 和 `step()` 之间共享状态

---

##第 4 步：运行控制循环

启动运行时，开始执行控制循环：

```bash
# 使用配置文件启动
plcopen-runtime --config config.yaml

# 或者使用命令行参数
plcopen-runtime --script control_script.py --cycle-time 100
```

**预期输出**:
```
[2026-01-17 10:30:00] INFO: PLCopen Runtime v1.0.0 启动
[2026-01-17 10:30:00] INFO: 加载配置: config.yaml
[2026-01-17 10:30:00] INFO: 加载用户脚本: control_script.py
[INIT] PID 控制器已初始化
[INIT] 目标温度: 25.0°C
[INIT] PID 参数: {'Kp': 2.0, 'Ki': 0.5, 'Kd': 0.1, ...}
[2026-01-17 10:30:00] INFO: 控制循环已启动，周期: 100ms
[周期 0010] 温度:  20.45°C | 误差: +4.55°C | 控制:  85.2%
[周期 0020] 温度:  21.83°C | 误差: +3.17°C | 控制:  72.1%
[周期 0030] 温度:  23.21°C | 误差: +1.79°C | 控制:  58.3%
[周期 0040] 温度:  24.15°C | 误差: +0.85°C | 控制:  47.6%
[周期 0050] 温度:  24.72°C | 误差: +0.28°C | 控制:  42.3%
[周期 0060] 温度:  24.95°C | 误差: +0.05°C | 控制:  39.8%
...
```

**停止运行时**:
```bash
# 按 Ctrl+C 优雅停止
^C
[2026-01-17 10:35:00] INFO: 收到停止信号
[2026-01-17 10:35:00] INFO: 正在停止控制循环...
[2026-01-17 10:35:00] INFO: 运行时已安全退出
```

---

## 第 5 步：使用远程调试

### 5.1 启用调试模式

修改 `config.yaml`：
```yaml
debug:
  enabled: true
  host: "0.0.0.0"     # 允许远程连接
  port: 5678
```

重新启动运行时：
```bash
plcopen-runtime --config config.yaml
```

**输出包含调试信息**:
```
[2026-01-17 10:40:00] INFO: 调试服务器已启动: 0.0.0.0:5678
[2026-01-17 10:40:00] INFO: 等待调试器连接...
```

### 5.2 使用 VS Code 连接

#### 安装 Python 扩展
在 VS Code 中安装 "Python" 扩展（Microsoft 官方）。

#### 创建 `.vscode/launch.json`
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "附加到 PLCopen 运行时",
      "type": "python",
      "request": "attach",
      "connect": {
        "host": "192.168.1.100",  // 嵌入式设备 IP
        "port": 5678
      },
      "pathMappings": [
        {
          "localRoot": "${workspaceFolder}",
          "remoteRoot": "/home/user/py-embedded-control"
        }
      ],
      "justMyCode": false
    }
  ]
}
```

#### 开始调试
1. 在 `control_script.py` 的 `step()` 函数中设置断点
2. 按 `F5` 或点击 "调试" → "开始调试"
3. VS Code 连接到运行时，断点处暂停
4. 使用调试工具栏检查变量、单步执行

**调试功能**:
- 断点、条件断点
- 变量监视（查看 `current_temperature`、`control_output` 等）
- 表达式求值（例如 `pid_controller.get_state()`）
- 调用堆栈查看

---

## 第 6 步：查看日志

运行时日志保存在 `runtime.log`（配置文件中定义）：

```bash
# 实时查看日志
tail -f runtime.log

# 搜索错误
grep ERROR runtime.log

# 统计周期抖动
grep "cycle jitter" runtime.log | awk '{sum+=$NF; count++} END {print "平均抖动:", sum/count, "ms"}'
```

**日志格式示例**:
```
2026-01-17 10:30:00.123 [INFO] runtime.c:45 - 控制循环已启动
2026-01-17 10:30:00.223 [DEBUG] pid.c:78 - PID 计算: e=4.55, P=9.10, I=2.28, D=0.05
2026-01-17 10:30:01.324 [WARNING] scheduler.c:102 - 周期抖动: 3.2ms (超过 1ms)
```

---

## 常见问题

### Q1: 运行时启动失败，提示 "ImportError: cannot import name 'plcopen'"

**原因**: Python 包未正确安装。

**解决方案**:
```bash
# 重新安装
cd python
pip3 install -e . --force-reinstall

# 检查安装位置
pip3 show plcopen-runtime
```

### Q2: 控制循环周期不稳定，日志显示大量 "cycle jitter" 警告

**原因**: 系统负载过高或配置不当。

**解决方案**:
1. 减少其他进程负载
2. 启用 CPU 亲和性：在 `config.yaml` 中设置 `cpu_affinity: 0`
3. 增加 `cycle_time`（例如从 10ms 改为 50ms）

### Q3: 远程调试连接失败

**原因**: 防火墙阻止或 IP 地址错误。

**解决方案**:
```bash
# 检查端口是否开放
netstat -tuln | grep 5678

# 检查防火墙（Ubuntu/Debian）
sudo ufw allow 5678/tcp

# 测试连接
telnet 192.168.1.100 5678
```

### Q4: PID 控制输出振荡（超调严重）

**原因**: PID 参数未正确整定。

**解决方案**:
1. 使用 Ziegler-Nichols 方法整定
2. 先从纯比例控制开始（`Ki=0`, `Kd=0`）
3. 参考日志中的误差变化曲线，逐步增加 `Ki` 和 `Kd`

---

## 下一步

完成快速入门后，你可以：

1. **学习更多功能块**: 参考 [API 文档](contracts/function_blocks_api.md) 了解 `FirstOrder`、`Ramp`、`Limit` 等功能块
2. **优化 PID 参数**: 使用 [PID 整定指南](docs/pid-tuning.md)
3. **集成真实硬件**: 参考 [硬件接口指南](docs/hardware-integration.md) 连接传感器和执行器
4. **部署生产环境**: 参考 [部署指南](docs/deployment.md) 配置 systemd 服务

---

## 技术支持

- **文档**: [https://docs.hollysys-cn.com/py-embedded-control](https://docs.hollysys-cn.com/py-embedded-control)
- **问题反馈**: [GitHub Issues](https://github.com/hollysys-cn/py-embedded-control/issues)
- **邮件支持**: support@hollysys-cn.com

---

**版权声明**: © 2026 Hollysys CN. 本文档遵循 MIT 许可证。
