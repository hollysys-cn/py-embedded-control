#!/usr/bin/env python3
"""测试 plcopen_c 模块"""

try:
    import plcopen_c
    print("✓ plcopen_c 模块导入成功")
    print(f"  模块路径: {plcopen_c.__file__}")
    print(f"  可用类: {[x for x in dir(plcopen_c) if not x.startswith('_')]}")

    # 测试 PID
    pid = plcopen_c.PID()
    print("✓ PID 对象创建成功")
    pid.set_params(Kp=1.0, Ki=0.1, Kd=0.01)
    result = pid.compute(100.0, 80.0)  # (setpoint, measurement)
    print(f"✓ PID 计算成功: output={result:.2f}")
    params = pid.get_params()
    print(f"  参数: Kp={params['Kp']}, Ki={params['Ki']}, Kd={params['Kd']}")

    # 测试 FirstOrder
    fo = plcopen_c.FirstOrder()
    print("✓ FirstOrder 对象创建成功")
    fo.set_time_constant(1.0)
    result = fo.compute(100.0)  # (input_value)
    print(f"✓ FirstOrder 计算成功: output={result:.2f}")
    params = fo.get_params()
    print(f"  参数: T={params['T']}")

    # 测试 Ramp
    ramp = plcopen_c.Ramp()
    print("✓ Ramp 对象创建成功")
    ramp.set_params(rising_rate=10.0, falling_rate=5.0)
    result = ramp.compute(input=100.0, dt=0.1)
    print(f"✓ Ramp 计算成功: output={result:.2f}")
    params = ramp.get_params()
    print(f"  参数: rising_rate={params['rising_rate']}, falling_rate={params['falling_rate']}")

    # 测试 Limit
    limit = plcopen_c.Limit()
    print("✓ Limit 对象创建成功")
    limit.set_params(min_value=0.0, max_value=100.0)
    result = limit.compute(input=150.0)
    print(f"✓ Limit 计算成功: output={result:.2f} (限幅到100.0)")

    print("\n所有测试通过！")

except Exception as e:
    print(f"✗ 错误: {e}")
    import traceback
    traceback.print_exc()
