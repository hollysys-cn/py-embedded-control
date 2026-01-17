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
 * @file context.c
 * @brief 运行时上下文管理实现
 */

#include "context.h"
#include "config_loader.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

// 全局运行时上下文（单例）
static RuntimeContext g_runtime_context = {0};
static int g_context_initialized = 0;

RuntimeContext* runtime_context_get(void) {
    return &g_runtime_context;
}

int runtime_context_init(const char* config_file) {
    if (g_context_initialized) {
        LOG_WARNING_MSG("运行时上下文已初始化");
        return 0;
    }

    fprintf(stdout, "DEBUG: Loading config from %s\n", config_file);
    fflush(stdout);

    // 加载配置
    if (config_load_from_file(config_file, &g_runtime_context.config) != 0) {
        fprintf(stderr, "配置加载失败：%s\n", config_file);
        fflush(stderr);
        return -1;
    }

    fprintf(stdout, "DEBUG: Config loaded, initializing logger\n");
    fflush(stdout);

    // 初始化日志系统
    if (logger_init(&g_runtime_context.config.log_config) != 0) {
        fprintf(stderr, "日志系统初始化失败\n");
        fflush(stderr);
        return -1;
    }

    LOG_INFO_MSG("运行时上下文初始化：配置文件=%s", config_file);
    fprintf(stdout, "DEBUG: Logger initialized, initializing Python\n");
    fflush(stdout);

    // 初始化 Python 解释器
    if (py_embed_init() != 0) {
        LOG_ERROR_MSG("Python 解释器初始化失败");
        logger_cleanup();
        return -1;
    }

    fprintf(stdout, "DEBUG: Python initialized, loading script: %s\n", g_runtime_context.config.script_path);
    fflush(stdout);

    // 加载用户脚本
    if (py_embed_load_script(g_runtime_context.config.script_path,
                             &g_runtime_context.py_context) != 0) {
        LOG_ERROR_MSG("用户脚本加载失败：%s", g_runtime_context.config.script_path);
        py_embed_cleanup();
        logger_cleanup();
        return -1;
    }

    fprintf(stdout, "DEBUG: Script loaded successfully\n");
    fflush(stdout);

    g_runtime_context.running = 0;
    g_runtime_context.cycle_count = 0;
    g_context_initialized = 1;

    LOG_INFO_MSG("运行时上下文初始化完成");

    return 0;
}

void runtime_context_cleanup(void) {
    if (!g_context_initialized) {
        return;
    }

    LOG_INFO_MSG("清理运行时上下文");

    g_runtime_context.running = 0;

    // 清理 Python 解释器
    py_embed_cleanup();

    // 清理日志系统
    logger_cleanup();

    g_context_initialized = 0;
}
