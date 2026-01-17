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
 * @file fb_ramp.c
 * @brief Ramp 斜率限制功能块实现
 */

#include "fb_ramp.h"
#include <math.h>
#include <string.h>

int ramp_init(RampFB* fb, double rising_rate, double falling_rate) {
    if (!fb) {
        return -1;
    }

    /* 参数验证 */
    if (rising_rate < 0.0 || falling_rate < 0.0) {
        return -1;
    }

    /* 初始化基础结构 */
    fb->base.type = FB_TYPE_RAMP;
    fb->base.id = 0;
    fb->base.last_update_time = 0.0;

    /* 设置参数 */
    fb->rising_rate = rising_rate;
    fb->falling_rate = falling_rate;

    /* 初始化状态 */
    fb->output = 0.0;
    fb->initialized = 0;

    return 0;
}

double ramp_compute(RampFB* fb, double input, double dt) {
    if (!fb || dt <= 0.0) {
        return 0.0;
    }

    /* 首次调用：直接使用输入值 */
    if (!fb->initialized) {
        fb->output = input;
        fb->initialized = 1;
        return fb->output;
    }

    /* 计算误差 */
    double error = input - fb->output;

    /* 计算最大允许变化量 */
    double max_change;
    if (error > 0.0) {
        /* 上升：使用上升速率 */
        max_change = fb->rising_rate * dt;
    } else {
        /* 下降：使用下降速率 */
        max_change = fb->falling_rate * dt;
    }

    /* 限制变化量 */
    double abs_error = fabs(error);
    if (abs_error <= max_change) {
        /* 误差小于限制，直接使用输入值 */
        fb->output = input;
    } else {
        /* 误差大于限制，按速率变化 */
        if (error > 0.0) {
            fb->output += max_change;
        } else {
            fb->output -= max_change;
        }
    }

    return fb->output;
}

int ramp_set_params(RampFB* fb, double rising_rate, double falling_rate) {
    if (!fb) {
        return -1;
    }

    /* 参数验证 */
    if (rising_rate < 0.0 || falling_rate < 0.0) {
        return -1;
    }

    fb->rising_rate = rising_rate;
    fb->falling_rate = falling_rate;

    return 0;
}

void ramp_get_params(const RampFB* fb, double* rising_rate, double* falling_rate) {
    if (!fb) {
        return;
    }

    if (rising_rate) {
        *rising_rate = fb->rising_rate;
    }
    if (falling_rate) {
        *falling_rate = fb->falling_rate;
    }
}

void ramp_reset(RampFB* fb, double initial_value) {
    if (!fb) {
        return;
    }


    fb->output = initial_value;
    fb->initialized = 1;
}
