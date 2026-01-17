"""
PLCopen 功能块高层 Python 封装

提供更友好的 Python 接口，封装 C 扩展模块。
"""

from typing import Dict, Optional

# 导入 C 扩展模块（Phase 3 完成后取消注释）
# import plcopen_c


class PID:
    """
    PID 控制器

    标准位置式 PID 算法：CV = Kp*e + Ki*∫e + Kd*de/dt

    参数:
        Kp: 比例系数，默认 1.0，范围 [0, 1e6]
        Ki: 积分系数，默认 0.0，范围 [0, 1e6]
        Kd: 微分系数，默认 0.0，范围 [0, 1e6]
        output_min: 输出下限，默认 -1e6
        output_max: 输出上限，默认 1e6

    示例:
        >>> pid = PID(Kp=2.0, Ki=0.5, Kd=0.1, output_min=0, output_max=100)
        >>> control = pid.compute(SP=25.0, PV=23.5)
        >>> print(f"控制输出: {control:.2f}")
    """

    def __init__(
        self,
        Kp: float = 1.0,
        Ki: float = 0.0,
        Kd: float = 0.0,
        output_min: float = -1e6,
        output_max: float = 1e6,
    ):
        # self._pid = plcopen_c.PID(Kp, Ki, Kd, output_min, output_max)
        # 临时占位：Phase 3 完成后使用真实 C 扩展
        self._params = {
            "Kp": Kp,
            "Ki": Ki,
            "Kd": Kd,
            "output_min": output_min,
            "output_max": output_max,
        }
        self._state = {"integral": 0.0, "prev_error": 0.0}

    def compute(self, SP: float, PV: float) -> float:
        """
        计算 PID 控制输出

        参数:
            SP: 设定值（Set Point）
            PV: 过程变量/反馈值（Process Variable）

        返回:
            float: 控制变量（Control Variable）
        """
        # return self._pid.compute(SP, PV)
        # 临时占位：简化实现
        error = SP - PV
        return self._params["Kp"] * error

    def set_params(
        self,
        Kp: Optional[float] = None,
        Ki: Optional[float] = None,
        Kd: Optional[float] = None,
    ) -> None:
        """
        运行时动态修改 PID 参数

        参数:
            Kp, Ki, Kd: 可选，仅修改提供的参数
        """
        # self._pid.set_params(Kp=Kp, Ki=Ki, Kd=Kd)
        if Kp is not None:
            self._params["Kp"] = Kp
        if Ki is not None:
            self._params["Ki"] = Ki
        if Kd is not None:
            self._params["Kd"] = Kd

    def get_params(self) -> Dict[str, float]:
        """
        获取当前 PID 参数

        返回:
            dict: {Kp, Ki, Kd, output_min, output_max}
        """
        # return self._pid.get_params()
        return self._params.copy()

    def get_state(self) -> Dict[str, float]:
        """
        获取内部状态（用于诊断）

        返回:
            dict: {integral, prev_error, last_error}
        """
        # return self._pid.get_state()
        return self._state.copy()

    def reset(self) -> None:
        """重置 PID 内部状态（积分清零）"""
        # self._pid.reset()
        self._state = {"integral": 0.0, "prev_error": 0.0}


class FirstOrder:
    """
    一阶惯性功能块

    传递函数：H(s) = 1 / (T*s + 1)

    参数:
        T: 时间常数（秒），默认 1.0，范围 [0.001, 1e6]

    示例:
        >>> fo = FirstOrder(T=0.5)
        >>> output = fo.compute(input=10.0)
        >>> print(f"输出: {output:.2f}")
    """

    def __init__(self, T: float = 1.0):
        # self._fo = plcopen_c.FirstOrder(T)
        # 临时占位
        self._params = {"T": T}
        self._prev_output = 0.0

    def compute(self, input: float) -> float:
        """
        计算一阶惯性输出

        参数:
            input: 输入信号

        返回:
            float: 输出信号
        """
        # return self._fo.compute(input)
        # 临时占位：简化实现（直接返回输入）
        alpha = 0.1  # 简化的 alpha
        output = alpha * input + (1 - alpha) * self._prev_output
        self._prev_output = output
        return output

    def set_time_constant(self, T: float) -> None:
        """
        动态修改时间常数

        参数:
            T: 时间常数（秒）
        """
        # self._fo.set_time_constant(T)
        self._params["T"] = T

    def get_params(self) -> Dict[str, float]:
        """
        获取参数

        返回:
            dict: {T}
        """
        # return self._fo.get_params()
        return self._params.copy()

    def reset(self) -> None:
        """重置内部状态（输出清零）"""
        # self._fo.reset()
        self._prev_output = 0.0


class Ramp:
    """
    斜率限制功能块

    限制信号的变化速率，防止突变。

    参数:
        rising_rate: 上升速率（单位/秒），默认 1.0
        falling_rate: 下降速率（单位/秒），默认 1.0

    示例:
        >>> ramp = Ramp(rising_rate=10.0, falling_rate=5.0)
        >>> output = ramp.compute(input=100.0, dt=0.1)
        >>> print(f"输出: {output:.2f}")
    """

    def __init__(self, rising_rate: float = 1.0, falling_rate: float = 1.0):
        self._params = {"rising_rate": rising_rate, "falling_rate": falling_rate}
        self._output = 0.0
        self._initialized = False

    def compute(self, input: float, dt: float) -> float:
        """
        计算斜率限制后的输出

        参数:
            input: 输入信号
            dt: 时间步长（秒）

        返回:
            float: 输出信号
        """
        if not self._initialized:
            self._output = input
            self._initialized = True
            return self._output

        error = input - self._output
        max_change = (
            self._params["rising_rate"] if error > 0 else self._params["falling_rate"]
        ) * dt

        if abs(error) <= max_change:
            self._output = input
        else:
            self._output += max_change if error > 0 else -max_change

        return self._output

    def set_params(
        self, rising_rate: Optional[float] = None, falling_rate: Optional[float] = None
    ) -> None:
        """
        动态修改斜率参数

        参数:
            rising_rate: 上升速率（单位/秒）
            falling_rate: 下降速率（单位/秒）
        """
        if rising_rate is not None:
            self._params["rising_rate"] = rising_rate
        if falling_rate is not None:
            self._params["falling_rate"] = falling_rate

    def get_params(self) -> Dict[str, float]:
        """获取参数"""
        return self._params.copy()

    def reset(self, initial_value: float = 0.0) -> None:
        """重置状态"""
        self._output = initial_value
        self._initialized = True


class Limit:
    """
    限幅功能块

    将信号限制在指定范围内。

    参数:
        min_value: 最小值，默认 0.0
        max_value: 最大值，默认 100.0

    示例:
        >>> limit = Limit(min_value=0.0, max_value=100.0)
        >>> output = limit.compute(input=150.0)
        >>> print(f"输出: {output:.2f}")  # 100.0
    """

    def __init__(self, min_value: float = 0.0, max_value: float = 100.0):
        if min_value > max_value:
            raise ValueError("min_value must be <= max_value")
        self._params = {"min_value": min_value, "max_value": max_value}

    def compute(self, input: float) -> float:
        """
        计算限幅后的输出

        参数:
            input: 输入信号

        返回:
            float: 输出信号
        """
        return max(self._params["min_value"], min(self._params["max_value"], input))

    def set_params(
        self, min_value: Optional[float] = None, max_value: Optional[float] = None
    ) -> None:
        """
        动态修改限幅参数

        参数:
            min_value: 最小值
            max_value: 最大值
        """
        if min_value is not None:
            self._params["min_value"] = min_value
        if max_value is not None:
            self._params["max_value"] = max_value

        if self._params["min_value"] > self._params["max_value"]:
            raise ValueError("min_value must be <= max_value")

    def get_params(self) -> Dict[str, float]:
        """获取参数"""
        return self._params.copy()


__all__ = ["PID", "FirstOrder", "Ramp", "Limit"]
