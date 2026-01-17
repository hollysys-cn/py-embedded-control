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
 * @file fb_first_order.c
 * @brief 一阶惯性功能块实现
 */

#include "fb_first_order.h"
#include "../runtime/logger.h"
#include <stdlib.h>
#include <time.h>

// 时间常数范围
#define T_MIN 0.001
#define T_MAX 1e6

// 全局实例计数器
static uint32_t g_fo_id_counter = 0;

FirstOrderFunctionBlock* first_order_create(double T) {
    // 分配内存
    FirstOrderFunctionBlock* fo = (FirstOrderFunctionBlock*)malloc(sizeof(FirstOrderFunctionBlock));
    if (!fo) {
        LOG_ERROR_MSG("一阶惯性创建失败：内存分配失败");
        return NULL;
    }

    // 初始化基础属性
    fo->base.type = FB_TYPE_FIRST_ORDER;
    fo->base.id = ++g_fo_id_counter;
    fo->base.last_update_time = 0.0;

    // 验证并设置参数
    fo->params.T = validate_and_clamp(T, T_MIN, T_MAX, "T");

    // 初始化状态
    fo->state.prev_output = 0.0;

    LOG_INFO_MSG("一阶惯性创建成功：ID=%u, T=%.3f", fo->base.id, fo->params.T);

    return fo;
}

void first_order_destroy(FirstOrderFunctionBlock* fo) {
    if (fo) {
        LOG_INFO_MSG("一阶惯性销毁：ID=%u", fo->base.id);
        free(fo);
    }
}

double first_order_compute(FirstOrderFunctionBlock* fo, double input, double dt) {
    if (!fo) {
        return 0.0;
    }

    // 如果 dt 为 0，自动计算时间差
    if (dt <= 0.0) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double current_time = now.tv_sec + now.tv_nsec / 1e9;

        if (fo->base.last_update_time > 0.0) {
            dt = current_time - fo->base.last_update_time;
        } else {
            dt = 0.1;  // 默认 100ms
        }

        fo->base.last_update_time = current_time;
    }

    // 计算 alpha = dt / (T + dt)
    double alpha = dt / (fo->params.T + dt);

    // 计算输出：Output = alpha * Input + (1 - alpha) * prev_output
    double output = alpha * input + (1.0 - alpha) * fo->state.prev_output;

    // 更新状态
    fo->state.prev_output = output;

    return output;
}

int first_order_set_time_constant(FirstOrderFunctionBlock* fo, double T) {
    if (!fo) {
        return -1;
    }

    fo->params.T = validate_and_clamp(T, T_MIN, T_MAX, "T");

    LOG_INFO_MSG("一阶惯性参数更新：ID=%u, T=%.3f", fo->base.id, fo->params.T);

    return 0;
}

const FirstOrderParams* first_order_get_params(const FirstOrderFunctionBlock* fo) {
    return fo ? &fo->params : NULL;
}

void first_order_reset(FirstOrderFunctionBlock* fo) {
    if (fo) {
        fo->state.prev_output = 0.0;
        fo->base.last_update_time = 0.0;
        LOG_INFO_MSG("一阶惯性状态重置：ID=%u", fo->base.id);
    }
}
