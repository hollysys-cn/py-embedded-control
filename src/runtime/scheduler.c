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
 * @file scheduler.c
 * @brief 周期调度器实现
 */

#define _GNU_SOURCE  // 为 CPU_SET 等宏定义
#include "scheduler.h"
#include "logger.h"
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

// 内部函数：计算时间差（毫秒）
static double timespec_diff_ms(const struct timespec* end, const struct timespec* start);

// 内部函数：时间加法
static void timespec_add_ms(struct timespec* ts, int ms);

int scheduler_init(SchedulerContext* ctx, int cycle_period_ms, int timeout_threshold_percent) {
    if (!ctx || cycle_period_ms < 10 || cycle_period_ms > 1000) {
        LOG_ERROR_MSG("无效的调度器参数：cycle_period_ms=%d", cycle_period_ms);
        return -1;
    }

    memset(ctx, 0, sizeof(SchedulerContext));
    ctx->cycle_period_ms = cycle_period_ms;
    ctx->timeout_threshold_percent = timeout_threshold_percent;
    ctx->stats.min_cycle_time_ms = 999999.0;  // 初始化为大值
    ctx->running = 1;

    // 获取当前时间作为起始点
    if (clock_gettime(CLOCK_MONOTONIC, &ctx->next_wakeup) != 0) {
        LOG_ERROR_MSG("获取系统时间失败：%s", strerror(errno));
        return -1;
    }

    LOG_INFO_MSG("调度器初始化：周期=%d ms, 超时阈值=%d%%",
                 cycle_period_ms, timeout_threshold_percent);

    return 0;
}

int scheduler_set_cpu_affinity(int cpu_core) {
    if (cpu_core < 0) {
        LOG_DEBUG_MSG("CPU 亲和性未设置（cpu_core=%d）", cpu_core);
        return 0;  // 不绑定
    }

#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);

    pid_t tid = gettid();  // 获取当前线程 ID

    if (sched_setaffinity(tid, sizeof(cpu_set_t), &cpuset) != 0) {
        LOG_ERROR_MSG("设置 CPU 亲和性失败（CPU %d）：%s", cpu_core, strerror(errno));
        return -1;
    }

    LOG_INFO_MSG("已绑定到 CPU 核心 %d", cpu_core);
    return 0;
#else
    LOG_WARNING_MSG("当前平台不支持 CPU 亲和性设置");
    return -1;
#endif
}

int scheduler_wait_next_cycle(SchedulerContext* ctx) {
    if (!ctx || !ctx->running) {
        return -1;
    }

    // 计算下次唤醒时间
    timespec_add_ms(&ctx->next_wakeup, ctx->cycle_period_ms);

    // 使用 clock_nanosleep 高精度休眠
    int ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ctx->next_wakeup, NULL);
    if (ret != 0) {
        LOG_WARNING_MSG("调度器休眠被中断：%s", strerror(ret));
        return -1;
    }

    return 0;
}

void scheduler_cycle_start(SchedulerContext* ctx, struct timespec* start_time) {
    if (!ctx || !start_time) {
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, start_time);
}

double scheduler_cycle_end(SchedulerContext* ctx, const struct timespec* start_time) {
    if (!ctx || !start_time) {
        return 0.0;
    }

    // 获取结束时间
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // 计算实际周期时间
    double actual_time_ms = timespec_diff_ms(&end_time, start_time);

    // 更新统计
    ctx->stats.cycle_count++;

    // 计算移动平均
    ctx->stats.avg_cycle_time_ms =
        (ctx->stats.avg_cycle_time_ms * (ctx->stats.cycle_count - 1) + actual_time_ms)
        / ctx->stats.cycle_count;

    // 更新最大/最小值
    if (actual_time_ms > ctx->stats.max_cycle_time_ms) {
        ctx->stats.max_cycle_time_ms = actual_time_ms;
    }
    if (actual_time_ms < ctx->stats.min_cycle_time_ms) {
        ctx->stats.min_cycle_time_ms = actual_time_ms;
    }

    // 检查超时
    double threshold_ms = ctx->cycle_period_ms * ctx->timeout_threshold_percent / 100.0;
    if (actual_time_ms > threshold_ms) {
        ctx->stats.timeout_count++;
        LOG_WARNING_MSG("周期超时：实际 %.2f ms > 阈值 %.2f ms（周期 %d）",
                        actual_time_ms, threshold_ms, ctx->stats.cycle_count);
    }

    // 每 1000 个周期输出统计信息
    if (ctx->stats.cycle_count % 1000 == 0) {
        LOG_INFO_MSG("调度器统计：周期数=%llu, 平均=%.2f ms, 最大=%.2f ms, 最小=%.2f ms, 超时次数=%llu",
                     (unsigned long long)ctx->stats.cycle_count,
                     ctx->stats.avg_cycle_time_ms,
                     ctx->stats.max_cycle_time_ms,
                     ctx->stats.min_cycle_time_ms,
                     (unsigned long long)ctx->stats.timeout_count);
    }

    return actual_time_ms;
}

const SchedulerStats* scheduler_get_stats(const SchedulerContext* ctx) {
    return ctx ? &ctx->stats : NULL;
}

void scheduler_stop(SchedulerContext* ctx) {
    if (ctx) {
        ctx->running = 0;
        LOG_INFO_MSG("调度器停止");
    }
}

// 内部函数实现

static double timespec_diff_ms(const struct timespec* end, const struct timespec* start) {
    double sec_diff = (double)(end->tv_sec - start->tv_sec);
    double nsec_diff = (double)(end->tv_nsec - start->tv_nsec);
    return sec_diff * 1000.0 + nsec_diff / 1000000.0;
}

static void timespec_add_ms(struct timespec* ts, int ms) {
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (ms % 1000) * 1000000L;

    // 处理纳秒溢出
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec += ts->tv_nsec / 1000000000L;
        ts->tv_nsec %= 1000000000L;
    }
}
