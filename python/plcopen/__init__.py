"""
PLCopen Python 运行时环境

此包提供 PLCopen 标准功能块的 Python 接口，用于快速实现控制逻辑。
"""

__version__ = "0.1.0"
__author__ = "Hollysys"
__license__ = "MIT"

# 导出功能块类
from plcopen.blocks import PID, FirstOrder, Ramp, Limit

__all__ = [
    "__version__",
    "PID",
    "FirstOrder",
    "Ramp",
    "Limit",
]
