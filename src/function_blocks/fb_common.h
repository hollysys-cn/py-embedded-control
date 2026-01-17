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
 * @file fb_common.h
 * @brief PLCopen 功能块通用接口定义
 */

#ifndef FB_COMMON_H
#define FB_COMMON_H

#include <stdint.h>

// 功能块类型枚举
typedef enum {
    FB_TYPE_PID,           // PID 控制器
    FB_TYPE_FIRST_ORDER,   // 一阶惯性
    FB_TYPE_RAMP,          // 斜坡生成器
    FB_TYPE_LIMIT          // 限幅器
} FunctionBlockType;

// 功能块基础结构（所有功能块的共同属性）
typedef struct {
    FunctionBlockType type;  // 功能块类型
    uint32_t id;             // 实例 ID
    double last_update_time; // 上次更新时间（秒）
} FunctionBlock;

/**
 * @brief 将浮点数限制到指定范围
 * @param value 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限制后的值
 */
double clamp(double value, double min, double max);

/**
 * @brief 验证参数并限制到有效范围
 * @param value 参数值
 * @param min 最小值
 * @param max 最大值
 * @param param_name 参数名称（用于日志）
 * @return 限制后的值
 */
double validate_and_clamp(double value, double min, double max, const char* param_name);

#endif // FB_COMMON_H
