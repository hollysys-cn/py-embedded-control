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
 * @file fb_limit.h
 * @brief Limit 限幅功能块（IEC 61131-3）
 *
 * 将信号限制在指定范围内
 */

#ifndef FB_LIMIT_H
#define FB_LIMIT_H

#include "fb_common.h"

/**
 * @brief Limit 功能块数据结构
 */
typedef struct {
    FunctionBlock base;     /**< 基础功能块结构 */

    /* 参数 */
    double min_value;       /**< 最小值 */
    double max_value;       /**< 最大值 */
} LimitFB;

/**
 * @brief 初始化 Limit 功能块
 * @param fb Limit 功能块指针
 * @param min_value 最小值
 * @param max_value 最大值
 * @return 成功返回 0，失败返回 -1
 */
int limit_init(LimitFB* fb, double min_value, double max_value);

/**
 * @brief 执行 Limit 计算
 * @param fb Limit 功能块指针
 * @param input 输入值
 * @return 输出值（限幅后）
 */
double limit_compute(LimitFB* fb, double input);

/**
 * @brief 设置 Limit 参数
 * @param fb Limit 功能块指针
 * @param min_value 最小值
 * @param max_value 最大值
 * @return 成功返回 0，失败返回 -1
 */
int limit_set_params(LimitFB* fb, double min_value, double max_value);

/**
 * @brief 获取 Limit 参数
 * @param fb Limit 功能块指针
 * @param min_value 输出最小值
 * @param max_value 输出最大值
 */
void limit_get_params(const LimitFB* fb, double* min_value, double* max_value);

#endif /* FB_LIMIT_H */
