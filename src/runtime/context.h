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
 * @file context.h
 * @brief 运行时上下文管理
 *
 * 管理全局运行时状态，包括配置、Python 解释器和功能块列表。
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include "py_embed.h"

// 运行时上下文结构体
typedef struct {
    RuntimeConfig config;        // 运行时配置
    PyEmbedContext py_context;   // Python 上下文
    int running;                 // 运行状态标志
    uint64_t cycle_count;        // 周期计数
} RuntimeContext;

/**
 * @brief 获取全局运行时上下文（单例）
 * @return 运行时上下文指针
 */
RuntimeContext* runtime_context_get(void);

/**
 * @brief 初始化运行时上下文
 * @param config_file 配置文件路径
 * @return 0 成功，-1 失败
 */
int runtime_context_init(const char* config_file);

/**
 * @brief 清理运行时上下文
 */
void runtime_context_cleanup(void);

#endif // CONTEXT_H
