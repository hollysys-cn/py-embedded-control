/*
 * Copyright (c) 2026 Hollysys Co., Ltd.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file scheduler.h
 * @brief 周期调度器接口
 *
 * 提供高精度的周期调度功能，支持周期抖动监控。
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <time.h>

// 调度器统计信息
typedef struct {
    uint64_t cycle_count;          // 执行周期计数
    uint64_t timeout_count;        // 超时次数
    double avg_cycle_time_ms;      // 平均周期时间（毫秒）
    double max_cycle_time_ms;      // 最大周期时间（毫秒）
    double min_cycle_time_ms;      // 最小周期时间（毫秒）
} SchedulerStats;

// 调度器上下文
typedef struct {
    int cycle_period_ms;           // 目标周期时间（毫秒）
    int timeout_threshold_percent; // 超时阈值（百分比）
    SchedulerStats stats;          // 统计信息
    struct timespec next_wakeup;   // 下次唤醒时间
    int running;                   // 是否正在运行
} SchedulerContext;

/**
 * @brief 初始化调度器
 * @param ctx 调度器上下文
 * @param cycle_period_ms 周期时间（毫秒）
 * @param timeout_threshold_percent 超时阈值（百分比）
 * @return 0 成功，-1 失败
 */
int scheduler_init(SchedulerContext* ctx, int cycle_period_ms, int timeout_threshold_percent);

/**
 * @brief 等待下一个周期
 * @param ctx 调度器上下文
 * @return 0 成功，-1 失败
 */
int scheduler_wait_next_cycle(SchedulerContext* ctx);

/**
 * @brief 记录周期开始时间
 * @param ctx 调度器上下文
 * @param start_time 输出开始时间
 */
void scheduler_cycle_start(SchedulerContext* ctx, struct timespec* start_time);

/**
 * @brief 记录周期结束时间并更新统计
 * @param ctx 调度器上下文
 * @param start_time 周期开始时间
 * @return 实际周期时间（毫秒）
 */
double scheduler_cycle_end(SchedulerContext* ctx, const struct timespec* start_time);

/**
 * @brief 获取调度器统计信息
 * @param ctx 调度器上下文
 * @return 统计信息指针
 */
const SchedulerStats* scheduler_get_stats(const SchedulerContext* ctx);

/**
 * @brief 停止调度器
 * @param ctx 调度器上下文
 */
void scheduler_stop(SchedulerContext* ctx);

/**
 * @brief 设置 CPU 亲和性（绑定到指定 CPU 核心）
 * @param cpu_core CPU 核心编号（0 开始），-1 表示不绑定
 * @return 0 成功，-1 失败
 *
 * @note 仅在 Linux 系统上支持
 * @note 需要足够的权限（或 CAP_SYS_NICE capability）
 */
int scheduler_set_cpu_affinity(int cpu_core);

#endif // SCHEDULER_H
