#!/usr/bin/env python3
"""
周期稳定性基准测试

测量运行时的周期稳定性，验证 95% 的周期在 ±5% 范围内。
"""

import argparse
import subprocess
import re
import statistics
from collections import Counter


def parse_log_file(log_file: str) -> list[float]:
    """
    从日志文件中提取周期时间

    Args:
        log_file: 日志文件路径

    Returns:
        周期时间列表（毫秒）
    """
    cycle_times = []

    # 正则表达式匹配周期时间日志
    # 示例：[INFO] 周期完成：实际 102.3 ms
    pattern = re.compile(r"周期.*?实际\s+([\d.]+)\s*ms", re.IGNORECASE)

    try:
        with open(log_file, "r", encoding="utf-8") as f:
            for line in f:
                match = pattern.search(line)
                if match:
                    cycle_time = float(match.group(1))
                    cycle_times.append(cycle_time)
    except FileNotFoundError:
        print(f"错误：日志文件不存在：{log_file}")
        return []
    except Exception as e:
        print(f"解析日志文件失败：{e}")
        return []

    return cycle_times


def calculate_statistics(
    cycle_times: list[float], target_cycle_ms: float
) -> dict:
    """
    计算周期统计信息

    Args:
        cycle_times: 周期时间列表
        target_cycle_ms: 目标周期时间

    Returns:
        统计信息字典
    """
    if not cycle_times:
        return {}

    # 基本统计
    avg = statistics.mean(cycle_times)
    median = statistics.median(cycle_times)
    stddev = statistics.stdev(cycle_times) if len(cycle_times) > 1 else 0.0
    min_time = min(cycle_times)
    max_time = max(cycle_times)

    # 计算在 ±5% 范围内的周期数
    tolerance = target_cycle_ms * 0.05
    lower_bound = target_cycle_ms - tolerance
    upper_bound = target_cycle_ms + tolerance

    within_tolerance = sum(
        1 for t in cycle_times if lower_bound <= t <= upper_bound
    )
    percentage_within_tolerance = (within_tolerance / len(cycle_times)) * 100

    # 计算抖动（最大偏差）
    max_deviation = max(abs(t - target_cycle_ms) for t in cycle_times)
    max_deviation_percent = (max_deviation / target_cycle_ms) * 100

    return {
        "count": len(cycle_times),
        "avg": avg,
        "median": median,
        "stddev": stddev,
        "min": min_time,
        "max": max_time,
        "within_tolerance": within_tolerance,
        "percentage_within_tolerance": percentage_within_tolerance,
        "max_deviation": max_deviation,
        "max_deviation_percent": max_deviation_percent,
        "target": target_cycle_ms,
        "tolerance": tolerance,
    }


def print_report(stats: dict):
    """打印统计报告"""
    if not stats:
        print("错误：无统计数据")
        return

    print("\n" + "=" * 60)
    print("周期稳定性基准测试报告")
    print("=" * 60)

    print(f"\n目标周期: {stats['target']:.2f} ms")
    print(f"容差范围: ±{stats['tolerance']:.2f} ms "
          f"({stats['target'] - stats['tolerance']:.2f} - "
          f"{stats['target'] + stats['tolerance']:.2f} ms)")
    print(f"\n总周期数: {stats['count']}")

    print("\n基本统计:")
    print(f"  平均值: {stats['avg']:.2f} ms")
    print(f"  中位数: {stats['median']:.2f} ms")
    print(f"  标准差: {stats['stddev']:.2f} ms")
    print(f"  最小值: {stats['min']:.2f} ms")
    print(f"  最大值: {stats['max']:.2f} ms")

    print("\n稳定性分析:")
    print(f"  在容差范围内的周期: {stats['within_tolerance']} "
          f"({stats['percentage_within_tolerance']:.2f}%)")
    print(f"  最大偏差: {stats['max_deviation']:.2f} ms "
          f"({stats['max_deviation_percent']:.2f}%)")

    print("\n" + "=" * 60)

    # 判定结果
    if stats["percentage_within_tolerance"] >= 95.0:
        print("✓ 测试通过：95% 以上的周期在 ±5% 范围内")
        return 0
    else:
        print(f"✗ 测试失败：仅 {stats['percentage_within_tolerance']:.2f}% "
              f"的周期在 ±5% 范围内（要求 ≥95%）")
        return 1


def run_runtime_and_collect_data(
    config_file: str, duration_sec: int, log_file: str
) -> list[float]:
    """
    运行运行时并收集周期数据

    Args:
        config_file: 配置文件路径
        duration_sec: 运行时间（秒）
        log_file: 日志文件路径

    Returns:
        周期时间列表
    """
    print(f"运行运行时（{duration_sec} 秒）...")

    try:
        # 启动运行时（带超时）
        subprocess.run(
            ["./bin/plcopen_runtime", "--config", config_file],
            timeout=duration_sec,
            check=False,
        )
    except subprocess.TimeoutExpired:
        print("运行时已运行指定时间，正在停止...")
    except FileNotFoundError:
        print("错误：运行时程序不存在（./bin/plcopen_runtime）")
        return []
    except Exception as e:
        print(f"运行时执行失败：{e}")
        return []

    # 解析日志文件
    print(f"解析日志文件：{log_file}")
    return parse_log_file(log_file)


def main():
    parser = argparse.ArgumentParser(description="周期稳定性基准测试")
    parser.add_argument(
        "--config",
        default="config/pid_temperature.yaml",
        help="配置文件路径",
    )
    parser.add_argument(
        "--duration",
        type=int,
        default=60,
        help="运行时间（秒，默认 60）",
    )
    parser.add_argument(
        "--log-file",
        default="logs/runtime.log",
        help="日志文件路径",
    )
    parser.add_argument(
        "--target-cycle",
        type=float,
        default=100.0,
        help="目标周期时间（毫秒，默认 100）",
    )
    parser.add_argument(
        "--analyze-only",
        action="store_true",
        help="仅分析现有日志文件（不运行程序）",
    )

    args = parser.parse_args()

    # 获取周期数据
    if args.analyze_only:
        print(f"分析现有日志文件：{args.log_file}")
        cycle_times = parse_log_file(args.log_file)
    else:
        cycle_times = run_runtime_and_collect_data(
            args.config, args.duration, args.log_file
        )

    if not cycle_times:
        print("错误：未找到周期数据")
        return 1

    # 计算统计信息
    stats = calculate_statistics(cycle_times, args.target_cycle)

    # 打印报告
    return print_report(stats)


if __name__ == "__main__":
    exit(main())
