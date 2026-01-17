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
 * @file fb_ramp.h
 * @brief Ramp 斜率限制功能块（IEC 61131-3）
 *
 * 限制信号的变化速率，防止突变
 */

#ifndef FB_RAMP_H
#define FB_RAMP_H

#include "fb_common.h"

/**
 * @brief Ramp 功能块数据结构
 */
typedef struct {
    FunctionBlock base;     /**< 基础功能块结构 */

    /* 参数 */
    double rising_rate;     /**< 上升速率（单位/秒） */
    double falling_rate;    /**< 下降速率（单位/秒） */

    /* 状态 */
    double output;          /**< 当前输出值 */
    int initialized;        /**< 初始化标志 */
} RampFB;

/**
 * @brief 初始化 Ramp 功能块
 * @param fb Ramp 功能块指针
 * @param rising_rate 上升速率（单位/秒）
 * @param falling_rate 下降速率（单位/秒）
 * @return 成功返回 0，失败返回 -1
 */
int ramp_init(RampFB* fb, double rising_rate, double falling_rate);

/**
 * @brief 执行 Ramp 计算
 * @param fb Ramp 功能块指针
 * @param input 输入值
 * @param dt 时间步长（秒）
 * @return 输出值（斜率限制后）
 */
double ramp_compute(RampFB* fb, double input, double dt);

/**
 * @brief 设置 Ramp 参数
 * @param fb Ramp 功能块指针
 * @param rising_rate 上升速率（单位/秒）
 * @param falling_rate 下降速率（单位/秒）
 * @return 成功返回 0，失败返回 -1
 */
int ramp_set_params(RampFB* fb, double rising_rate, double falling_rate);

/**
 * @brief 获取 Ramp 参数
 * @param fb Ramp 功能块指针
 * @param rising_rate 输出上升速率
 * @param falling_rate 输出下降速率
 */
void ramp_get_params(const RampFB* fb, double* rising_rate, double* falling_rate);

/**
 * @brief 重置 Ramp 功能块
 * @param fb Ramp 功能块指针
 * @param initial_value 初始值
 */
void ramp_reset(RampFB* fb, double initial_value);

#endif /* FB_RAMP_H */
