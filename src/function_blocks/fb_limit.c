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
