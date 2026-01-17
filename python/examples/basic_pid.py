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
基础 PID 示例

演示如何使用 PID 控制器功能块进行简单的控制。
"""

import sys

from plcopen import PID

sys.path.insert(0, ".")
sys.path.insert(0, "./python")


def main():
    """主函数"""
    print("=" * 50)
    print("PLCopen PID 控制器基础示例")
    print("=" * 50)

    # 创建 PID 控制器
    # Kp=2.0: 比例系数
    # Ki=0.5: 积分系数
    # Kd=0.1: 微分系数
    # output_min=0, output_max=100: 输出范围 0-100%
    pid = PID(Kp=2.0, Ki=0.5, Kd=0.1, output_min=0, output_max=100)

    print(f"\nPID 参数: {pid.get_params()}")

    # 设定值（目标温度）
    setpoint = 25.0

    # 模拟控制过程（10 步）
    print(f"\n目标温度: {setpoint}°C")
    print("\n步骤 | 当前温度 | 误差   | 控制输出")
    print("-" * 45)

    # 初始温度
    temperature = 20.0

    for step in range(10):
        # 计算控制输出
        control = pid.compute(SP=setpoint, PV=temperature)

        # 显示结果
        error = setpoint - temperature
        print(f"{step+1:4d} | {temperature:8.2f} | {error:6.2f} | {control:8.2f}%")

        # 模拟温度变化（简化模型：温度向设定值靠近）
        temperature += error * 0.1

    print("\n" + "=" * 50)
    print("示例完成")
    print("=" * 50)


if __name__ == "__main__":
    main()
