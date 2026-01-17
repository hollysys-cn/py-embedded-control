/**
 * @file fb_common.c
 * @brief PLCopen 功能块通用实现
 */

#include "fb_common.h"
#include "../runtime/logger.h"
#include <math.h>

double clamp(double value, double min, double max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

double validate_and_clamp(double value, double min, double max, const char* param_name) {
    double original = value;
    double clamped = clamp(value, min, max);

    if (fabs(clamped - original) > 1e-9) {
        LOG_WARNING_MSG("参数 %s 超出范围：原值=%.6f, 限制后=%.6f (范围 [%.6f, %.6f])",
                        param_name, original, clamped, min, max);
    }

    return clamped;
}
