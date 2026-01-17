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
 * @file main.c
 * @brief PLCopen Python 运行时主程序
 *
 * 运行时主循环，负责周期性调用用户脚本。
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "context.h"
#include "scheduler.h"
#include "logger.h"
#include "py_embed.h"
#include "debug_server.h"
#include "debug_session.h"

// 全局信号标志
static volatile sig_atomic_t g_shutdown_requested = 0;

/**
 * @brief 信号处理函数
 */
static void signal_handler(int signum) {
    (void)signum;  // 抑制未使用警告
    g_shutdown_requested = 1;
}

/**
 * @brief 打印使用说明
 */
static void print_usage(const char* program_name) {
    printf("PLCopen Python 运行时环境 v0.1.0\n\n");
    printf("用法: %s [选项]\n\n", program_name);
    printf("选项:\n");
    printf("  --config FILE    配置文件路径（默认: config/runtime.yaml）\n");
    printf("  --help, -h       显示此帮助信息\n");
    printf("\n");
}

/**
 * @brief 主函数
 */
int main(int argc, char* argv[]) {
    const char* config_file = "config/runtime.yaml";

    // 添加调试输出，确保程序启动
    fprintf(stdout, "PLCopen Runtime starting...\n");
    fflush(stdout);

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_file = argv[++i];
            fprintf(stdout, "Using config: %s\n", config_file);
            fflush(stdout);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "未知选项: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    fprintf(stdout, "Initializing runtime context...\n");
    fflush(stdout);

    // 初始化运行时上下文
    if (runtime_context_init(config_file) != 0) {
        fprintf(stderr, "运行时初始化失败\n");
        fflush(stderr);
        return 1;
    }

    RuntimeContext* ctx = runtime_context_get();

    // 启动调试服务器（如果启用）
    DebugSession debug_session;
    if (ctx->config.debug_enabled) {
        if (debug_session_init(&debug_session,
                              ctx->config.debug_host,
                              ctx->config.debug_port,
                              ctx->config.debug_timeout) == 0) {
            // 启动 debugpy 服务器（失败不影响运行）
            if (debug_server_start(&debug_session, &ctx->py_context) != 0) {
                LOG_WARNING_MSG("调试服务器启动失败，程序继续运行");
            }
        }
    } else {
        LOG_DEBUG_MSG("调试功能未启用");
    }

    // 调用用户脚本的 init() 函数
    if (py_embed_call_init(&ctx->py_context) != 0) {
        LOG_ERROR_MSG("init() 函数调用失败");
        runtime_context_cleanup();
        return 1;
    }

    // 初始化调度器
    SchedulerContext scheduler;
    if (scheduler_init(&scheduler,
                      ctx->config.cycle_period_ms,
                      ctx->config.timeout_threshold_percent) != 0) {
        LOG_ERROR_MSG("调度器初始化失败");
        runtime_context_cleanup();
        return 1;
    }

    // 设置 CPU 亲和性（如果配置了）
    if (ctx->config.cpu_affinity >= 0) {
        scheduler_set_cpu_affinity(ctx->config.cpu_affinity);
    }

    ctx->running = 1;
    LOG_INFO_MSG("运行时启动：周期=%d ms", ctx->config.cycle_period_ms);

    // 主循环
    while (ctx->running && !g_shutdown_requested) {
        struct timespec cycle_start;

        // 记录周期开始时间
        scheduler_cycle_start(&scheduler, &cycle_start);

        // 检查调试服务器状态（如果启用）
        if (ctx->config.debug_enabled) {
            debug_server_check_status(&debug_session);
        }

        // 调用用户脚本的 step() 函数
        if (py_embed_call_step(&ctx->py_context) != 0) {
            LOG_ERROR_MSG("step() 函数执行失败");
            // 继续运行，不退出
        }

        // 记录周期结束时间并更新统计
        double actual_time = scheduler_cycle_end(&scheduler, &cycle_start);
        (void)actual_time;  // 抑制未使用警告

        ctx->cycle_count++;

        // 等待下一个周期
        if (scheduler_wait_next_cycle(&scheduler) != 0) {
            LOG_WARNING_MSG("调度器等待被中断");
        }
    }

    // 停止调试服务器（如果启用）
    if (ctx->config.debug_enabled) {
        debug_server_stop(&debug_session);
    }

    // 输出最终统计
    const SchedulerStats* stats = scheduler_get_stats(&scheduler);
    LOG_INFO_MSG("运行时停止：总周期=%llu, 平均周期=%.2f ms, 超时次数=%llu",
                 (unsigned long long)stats->cycle_count,
                 stats->avg_cycle_time_ms,
                 (unsigned long long)stats->timeout_count);

    // 清理
    scheduler_stop(&scheduler);
    runtime_context_cleanup();

    printf("运行时已正常退出\n");
    return 0;
}
