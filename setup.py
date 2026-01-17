"""
setup.py - Python C Extension 构建脚本

构建 PLCopen 功能块的 Python 绑定。
"""

from setuptools import setup, Extension
import os

# 源文件列表
c_sources = [
    # Python 绑定
    'src/python_bindings/plcopen_module.c',
    'src/python_bindings/py_pid.c',
    'src/python_bindings/py_first_order.c',
    'src/python_bindings/py_ramp.c',
    'src/python_bindings/py_limit.c',

    # 功能块实现
    'src/function_blocks/fb_common.c',
    'src/function_blocks/fb_pid.c',
    'src/function_blocks/fb_first_order.c',
    'src/function_blocks/fb_ramp.c',
    'src/function_blocks/fb_limit.c',

    # 运行时支持
    'src/runtime/logger.c',
]

# 编译选项
extra_compile_args = [
    '-std=c11',
    '-D_POSIX_C_SOURCE=200809L',
    '-Wall',
    '-Wextra',
    '-O2',
    '-fPIC',
]

# 链接选项
extra_link_args = [
    '-lpthread',  # pthread 用于日志系统的互斥锁
    '-lm',        # 数学库
]

# 定义扩展模块
plcopen_ext = Extension(
    'plcopen_c',  # 模块名
    sources=c_sources,
    include_dirs=['src'],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setup(
    name='plcopen',
    version='0.1.0',
    description='PLCopen 标准功能块 Python 运行时',
    author='Hollysys',
    license='MIT',
    ext_modules=[plcopen_ext],
    packages=['plcopen'],
    package_dir={'plcopen': 'python/plcopen'},
    python_requires='>=3.8',
    install_requires=[
        'debugpy>=1.6.0',
        'PyYAML>=6.0',
    ],
)
