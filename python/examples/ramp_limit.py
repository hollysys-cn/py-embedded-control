"""
Ramp 和 Limit 功能块示例

展示如何使用斜率限制和限幅功能块。
"""

import sys
sys.path.insert(0, '.')
sys.path.insert(0, './python')

from plcopen import Ramp, Limit

def main():
    print("=" * 60)
    print("Ramp 和 Limit 功能块示例")
    print("=" * 60)

    # 1. Ramp 示例：限制信号变化速率
    print("\n1. Ramp 功能块示例")
    print("-" * 60)

    ramp = Ramp(rising_rate=10.0, falling_rate=5.0)
    print(f"创建 Ramp: rising_rate=10.0 单位/秒, falling_rate=5.0 单位/秒")

    # 模拟突变输入信号
    test_input = 100.0
    dt = 0.1  # 100ms 周期

    print(f"\n输入突变信号: {test_input}")
    print(f"时间步长: {dt}s")
    print("\n时间(s) | 输入 | 输出 | 变化率")
    print("-" * 60)

    for i in range(15):
        time = i * dt
        output = ramp.compute(test_input, dt)
        rate = (output - (ramp._output if i > 0 else 0)) / dt if i > 0 else 0
        print(f"{time:6.2f}  | {test_input:5.1f} | {output:5.1f} | {rate:+6.2f} 单位/秒")

        # 第 8 个周期后，输入变为 0（下降）
        if i == 7:
            test_input = 0.0
            print(f"\n输入变为: {test_input}")
            print("\n时间(s) | 输入 | 输出 | 变化率")
            print("-" * 60)

    # 2. Limit 示例：限幅
    print("\n\n2. Limit 功能块示例")
    print("-" * 60)

    limit = Limit(min_value=0.0, max_value=100.0)
    print(f"创建 Limit: min=0.0, max=100.0")

    test_inputs = [-50, -10, 0, 25, 50, 75, 100, 120, 150]

    print("\n输入 | 输出 | 状态")
    print("-" * 60)
    for value in test_inputs:
        output = limit.compute(value)
        status = "限制在下限" if value < 0 else ("限制在上限" if value > 100 else "正常范围")
        print(f"{value:4.0f} | {output:5.1f} | {status}")

    # 3. 组合示例：Ramp + Limit
    print("\n\n3. Ramp + Limit 组合示例")
    print("-" * 60)

    ramp2 = Ramp(rising_rate=20.0, falling_rate=20.0)
    limit2 = Limit(min_value=10.0, max_value=90.0)

    print("Ramp 速率: 20.0 单位/秒")
    print("Limit 范围: [10.0, 90.0]")

    # 模拟突变输入
    target = 100.0
    dt = 0.1

    print(f"\n目标值: {target}")
    print("\n时间(s) | Ramp输出 | Limit输出")
    print("-" * 60)

    for i in range(10):
        time = i * dt
        ramp_out = ramp2.compute(target, dt)
        final_out = limit2.compute(ramp_out)
        print(f"{time:6.2f}  | {ramp_out:8.2f} | {final_out:10.2f}")

    print("\n" + "=" * 60)
    print("示例完成")
    print("=" * 60)

if __name__ == "__main__":
    main()
