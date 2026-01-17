# Copyright (c) 2026 Hollysys Co., Ltd.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
"""
PID 温度控制示例

模拟一个简单的加热系统，使用 PID 控制器将温度稳定在设定值。

物理模型：
- 加热器：控制信号 0-100% 对应加热功率
- 散热：温度越高散热越快
- 测量噪声：模拟传感器噪声
"""

import sys
import random

from plcopen import PID

sys.path.insert(0, ".")
sys.path.insert(0, "./python")

# 全局变量
pid = None
temperature = 20.0  # 当前温度（°C）
setpoint = 25.0  # 目标温度（°C）
step_count = 0


def init():
    """初始化函数，运行时启动时调用一次"""
    global pid

    # 创建 PID 控制器
    # Kp=2.5: 比例系数（适度的比例作用）
    # Ki=3.0: 积分系数（强积分作用，消除最后的稳态误差）
    # Kd=0.3: 微分系数（适度抑制超调）
    # output_min=0, output_max=100: 控制输出 0-100%
    pid = PID(Kp=2.5, Ki=3.0, Kd=0.3, output_min=0, output_max=100)

    print("PID 温度控制初始化完成")
    print(f"  目标温度: {setpoint}°C")
    print(f"  初始温度: {temperature}°C")
    print(
        f"  PID 参数: Kp={pid.get_params()['Kp']}, "
        f"Ki={pid.get_params()['Ki']}, "
        f"Kd={pid.get_params()['Kd']}"
    )
    print("-" * 60)


def step():
    """周期执行函数，每个控制周期调用"""
    global temperature, step_count

    if pid is None:
        raise RuntimeError("PID 控制器未初始化，请先调用 init()")

    step_count += 1

    # 添加测量噪声（模拟真实传感器）
    measured_temp = temperature + random.uniform(-0.1, 0.1)

    # 计算 PID 控制输出
    control_output = pid.compute(SP=setpoint, PV=measured_temp)

    # 物理模型仿真
    # 1. 加热效果：控制输出越大，温度上升越快
    heating = control_output * 0.1  # 加热贡献

    # 2. 自然散热：温度越高，散热越快（牛顿冷却定律）
    ambient_temp = 20.0
    cooling = (temperature - ambient_temp) * 0.0065  # 散热贡献（精确调节以达到25°C）

    # 3. 更新温度（离散时间步进）
    temperature += heating - cooling

    # 每 10 个周期输出一次状态
    if step_count % 10 == 0:
        error = setpoint - temperature
        print(
            f"周期 {step_count:4d} | "
            f"温度: {temperature:5.2f}°C | "
            f"误差: {error:6.3f}°C | "
            f"控制输出: {control_output:5.1f}%"
        )

    # 每 100 个周期输出分隔线
    if step_count % 100 == 0:
        print("-" * 60)


if __name__ == "__main__":
    # 本地测试模式（不使用运行时）
    print("=" * 60)
    print("PID 温度控制本地测试")
    print("=" * 60)

    init()

    # 模拟 200 个周期（20秒），观察温度是否能稳定到目标值
    for _ in range(200):
        step()

    print("=" * 60)
    print(f"测试完成：最终温度 = {temperature:.2f}°C，误差 = {setpoint - temperature:.2f}°C")
    print("=" * 60)
