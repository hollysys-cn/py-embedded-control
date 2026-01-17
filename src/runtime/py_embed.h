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
 * @file py_embed.h
 * @brief Python 解释器嵌入接口
 *
 * 提供 Python 解释器初始化、脚本加载和执行功能。
 */

#ifndef PY_EMBED_H
#define PY_EMBED_H

#include <Python.h>

// Python 嵌入上下文
typedef struct {
    PyObject* module;          // 用户脚本模块
    PyObject* init_func;       // init() 函数
    PyObject* step_func;       // step() 函数
    int initialized;           // 是否已初始化
} PyEmbedContext;

/**
 * @brief 初始化 Python 解释器
 * @return 0 成功，-1 失败
 */
int py_embed_init(void);

/**
 * @brief 清理 Python 解释器资源
 */
void py_embed_cleanup(void);

/**
 * @brief 加载用户脚本
 * @param script_path 脚本文件路径
 * @param context 输出上下文
 * @return 0 成功，-1 失败
 */
int py_embed_load_script(const char* script_path, PyEmbedContext* context);

/**
 * @brief 调用用户脚本的 init() 函数
 * @param context Python 上下文
 * @return 0 成功，-1 失败
 */
int py_embed_call_init(PyEmbedContext* context);

/**
 * @brief 调用用户脚本的 step() 函数
 * @param context Python 上下文
 * @return 0 成功，-1 失败
 */
int py_embed_call_step(PyEmbedContext* context);

/**
 * @brief 处理 Python 异常
 *
 * 捕获并记录 Python 异常，防止崩溃。
 */
void py_embed_handle_exception(void);

#endif // PY_EMBED_H
