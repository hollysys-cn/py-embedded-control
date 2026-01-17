#!/usr/bin/env python3

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
debugpy 调试服务器脚本

这是一个独立的 Python 脚本，用于演示如何在 Python 环境中启动 debugpy 服务器。
在实际的 C 运行时环境中，调试服务器由 debug_server.c 通过 Python C API 启动。
"""

import sys
import time


def start_debug_server(host: str = "0.0.0.0", port: int = 5678):
    """
    启动 debugpy 调试服务器

    Args:
        host: 监听地址
        port: 监听端口
    """
    try:
        import debugpy
    except ImportError:
        print("错误：debugpy 未安装", file=sys.stderr)
        print("请运行: pip install debugpy", file=sys.stderr)
        return False

    try:
        print(f"启动 debugpy 服务器: {host}:{port}")
        debugpy.listen((host, port))
        print(f"debugpy 服务器已启动，等待调试器连接...")
        print(f"VS Code 配置: \"host\": \"{host}\", \"port\": {port}")
        return True
    except Exception as e:
        print(f"启动 debugpy 失败: {e}", file=sys.stderr)
        return False


def wait_for_client(timeout: int = 30):
    """
    等待调试客户端连接

    Args:
        timeout: 超时时间（秒），0 表示无限等待
    """
    try:
        import debugpy
    except ImportError:
        return False

    print(f"等待调试器连接（超时: {timeout}秒）...")

    try:
        if timeout > 0:
            debugpy.wait_for_client()
            print("调试器已连接")
        else:
            debugpy.wait_for_client()
            print("调试器已连接")
        return True
    except Exception as e:
        print(f"等待连接失败: {e}", file=sys.stderr)
        return False


def main():
    """主函数：演示调试服务器的启动和使用"""
    import argparse

    parser = argparse.ArgumentParser(description="启动 debugpy 调试服务器")
    parser.add_argument("--host", default="0.0.0.0", help="监听地址（默认: 0.0.0.0）")
    parser.add_argument("--port", type=int, default=5678, help="监听端口（默认: 5678）")
    parser.add_argument(
        "--wait", action="store_true", help="等待调试器连接后再继续"
    )
    parser.add_argument(
        "--timeout", type=int, default=30, help="等待超时时间（秒，默认: 30）"
    )

    args = parser.parse_args()

    # 启动调试服务器
    if not start_debug_server(args.host, args.port):
        sys.exit(1)

    # 等待连接（可选）
    if args.wait:
        if not wait_for_client(args.timeout):
            print("警告：调试器未连接，继续执行")

    # 模拟运行时环境
    print("\n开始执行用户代码...")
    print("（在 C 运行时中，这里会执行控制逻辑循环）")

    try:
        # 模拟周期执行
        for i in range(10):
            print(f"周期 {i + 1}: 执行控制逻辑")
            time.sleep(1)

            # 在此处可以设置断点进行调试
            result = i * 2
            print(f"  结果: {result}")

    except KeyboardInterrupt:
        print("\n程序被中断")

    print("\n程序执行完成")


if __name__ == "__main__":
    main()
