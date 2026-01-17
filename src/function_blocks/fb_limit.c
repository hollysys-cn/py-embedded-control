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
 * @file fb_limit.c
 * @brief Limit 限幅功能块实现
 */

#include "fb_limit.h"

int limit_init(LimitFB* fb, double min_value, double max_value) {
    if (!fb) {
        return -1;
    }

    /* 参数验证 */
    if (min_value > max_value) {
        return -1;
    }

    /* 初始化基础结构 */
    fb->base.type = FB_TYPE_LIMIT;
    fb->base.id = 0;
    fb->base.last_update_time = 0.0;

    /* 设置参数 */
    fb->min_value = min_value;
    fb->max_value = max_value;

    return 0;
}

double limit_compute(LimitFB* fb, double input) {
    if (!fb) {
        return 0.0;
    }

    double output;

    /* 限幅逻辑 */
    if (input < fb->min_value) {
        output = fb->min_value;
    } else if (input > fb->max_value) {
        output = fb->max_value;
    } else {
        output = input;
    }

    return output;
}

int limit_set_params(LimitFB* fb, double min_value, double max_value) {
    if (!fb) {
        return -1;
    }

    /* 参数验证 */
    if (min_value > max_value) {
        return -1;
    }

    fb->min_value = min_value;
    fb->max_value = max_value;

    return 0;
}

void limit_get_params(const LimitFB* fb, double* min_value, double* max_value) {
    if (!fb) {
        return;
    }

    if (min_value) {
        *min_value = fb->min_value;
    }
    if (max_value) {
        *max_value = fb->max_value;
    }
}
