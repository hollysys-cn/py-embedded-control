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
 * @file fb_first_order.h
 * @brief 一阶惯性功能块接口
 *
 * 传递函数：H(s) = 1 / (T*s + 1)
 * 离散化：Output = alpha * Input + (1 - alpha) * prev_output
 *         其中 alpha = dt / (T + dt)
 */

#ifndef FB_FIRST_ORDER_H
#define FB_FIRST_ORDER_H

#include "fb_common.h"

// 一阶惯性参数
typedef struct {
    double T;  // 时间常数（秒）[0.001, 1e6]
} FirstOrderParams;

// 一阶惯性状态
typedef struct {
    double prev_output;  // 上一周期输出值
} FirstOrderState;

// 一阶惯性功能块
typedef struct {
    FunctionBlock base;
    FirstOrderParams params;
    FirstOrderState state;
} FirstOrderFunctionBlock;

/**
 * @brief 创建一阶惯性功能块实例
 * @param T 时间常数（秒）
 * @return 功能块指针，失败返回 NULL
 */
FirstOrderFunctionBlock* first_order_create(double T);

/**
 * @brief 销毁一阶惯性功能块
 * @param fo 功能块指针
 */
void first_order_destroy(FirstOrderFunctionBlock* fo);

/**
 * @brief 计算一阶惯性输出
 * @param fo 功能块指针
 * @param input 输入信号
 * @param dt 时间步长（秒），0 表示自动计算
 * @return 输出信号
 */
double first_order_compute(FirstOrderFunctionBlock* fo, double input, double dt);

/**
 * @brief 设置时间常数
 * @param fo 功能块指针
 * @param T 时间常数
 * @return 0 成功，-1 失败
 */
int first_order_set_time_constant(FirstOrderFunctionBlock* fo, double T);

/**
 * @brief 获取参数
 * @param fo 功能块指针
 * @return 参数结构体指针
 */
const FirstOrderParams* first_order_get_params(const FirstOrderFunctionBlock* fo);

/**
 * @brief 重置状态
 * @param fo 功能块指针
 */
void first_order_reset(FirstOrderFunctionBlock* fo);

#endif // FB_FIRST_ORDER_H
