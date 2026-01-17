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
 * @file fb_pid.c
 * @brief PID 控制器功能块实现
 */

#include "fb_pid.h"
#include "../runtime/logger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// PID 参数范围
#define PID_PARAM_MIN 0.0
#define PID_PARAM_MAX 1e6

// 全局 PID 实例计数器（用于生成唯一 ID）
static uint32_t g_pid_id_counter = 0;

PIDFunctionBlock* pid_create(double Kp, double Ki, double Kd,
                              double output_min, double output_max) {
    // 验证输出范围
    if (output_min >= output_max) {
        LOG_ERROR_MSG("PID 创建失败：output_min (%.6f) >= output_max (%.6f)",
                      output_min, output_max);
        return NULL;
    }

    // 分配内存
    PIDFunctionBlock* pid = (PIDFunctionBlock*)malloc(sizeof(PIDFunctionBlock));
    if (!pid) {
        LOG_ERROR_MSG("PID 创建失败：内存分配失败");
        return NULL;
    }

    // 初始化基础属性
    pid->base.type = FB_TYPE_PID;
    pid->base.id = ++g_pid_id_counter;
    pid->base.last_update_time = 0.0;

    // 验证并设置参数
    pid->params.Kp = validate_and_clamp(Kp, PID_PARAM_MIN, PID_PARAM_MAX, "Kp");
    pid->params.Ki = validate_and_clamp(Ki, PID_PARAM_MIN, PID_PARAM_MAX, "Ki");
    pid->params.Kd = validate_and_clamp(Kd, PID_PARAM_MIN, PID_PARAM_MAX, "Kd");
    pid->params.output_min = output_min;
    pid->params.output_max = output_max;

    // 初始化状态
    pid->state.integral = 0.0;
    pid->state.prev_error = 0.0;
    pid->last_error = 0.0;

    LOG_INFO_MSG("PID 控制器创建成功：ID=%u, Kp=%.3f, Ki=%.3f, Kd=%.3f, 输出范围=[%.2f, %.2f]",
                 pid->base.id, pid->params.Kp, pid->params.Ki, pid->params.Kd,
                 pid->params.output_min, pid->params.output_max);

    return pid;
}

void pid_destroy(PIDFunctionBlock* pid) {
    if (pid) {
        LOG_INFO_MSG("PID 控制器销毁：ID=%u", pid->base.id);
        free(pid);
    }
}

double pid_compute(PIDFunctionBlock* pid, double SP, double PV, double dt) {
    if (!pid) {
        return 0.0;
    }

    // 计算误差
    double error = SP - PV;

    // 如果 dt 为 0，自动计算时间差
    if (dt <= 0.0) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double current_time = now.tv_sec + now.tv_nsec / 1e9;

        if (pid->base.last_update_time > 0.0) {
            dt = current_time - pid->base.last_update_time;
        } else {
            dt = 0.1;  // 默认 100ms
        }

        pid->base.last_update_time = current_time;
    }

    // 比例项（直接计算，避免临时变量）
    double output = pid->params.Kp * error;

    // 积分项（累积误差）
    pid->state.integral += error * dt;
    output += pid->params.Ki * pid->state.integral;

    // 微分项（基于误差变化率）
    if (dt > 0.0) {
        double derivative = (error - pid->state.prev_error) / dt;
        output += pid->params.Kd * derivative;
    }

    // 更新状态（最小化写操作）
    pid->state.prev_error = error;
    pid->last_error = error;

    // 输出限幅（内联函数）
    double limited_output = clamp(output, pid->params.output_min, pid->params.output_max);

    // 抗积分饱和（仅在输出饱和且启用积分时执行）
    if (__builtin_expect(limited_output != output && pid->params.Ki > 0.0, 0)) {
        pid->state.integral -= error * dt;  // 回退积分
    }

    return limited_output;
}

int pid_set_params(PIDFunctionBlock* pid, const double* Kp,
                   const double* Ki, const double* Kd) {
    if (!pid) {
        return -1;
    }

    if (Kp) {
        pid->params.Kp = validate_and_clamp(*Kp, PID_PARAM_MIN, PID_PARAM_MAX, "Kp");
    }
    if (Ki) {
        pid->params.Ki = validate_and_clamp(*Ki, PID_PARAM_MIN, PID_PARAM_MAX, "Ki");
    }
    if (Kd) {
        pid->params.Kd = validate_and_clamp(*Kd, PID_PARAM_MIN, PID_PARAM_MAX, "Kd");
    }

    LOG_INFO_MSG("PID 参数更新：ID=%u, Kp=%.3f, Ki=%.3f, Kd=%.3f",
                 pid->base.id, pid->params.Kp, pid->params.Ki, pid->params.Kd);

    return 0;
}

const PIDParams* pid_get_params(const PIDFunctionBlock* pid) {
    return pid ? &pid->params : NULL;
}

const PIDState* pid_get_state(const PIDFunctionBlock* pid) {
    return pid ? &pid->state : NULL;
}

void pid_reset(PIDFunctionBlock* pid) {
    if (pid) {
        pid->state.integral = 0.0;
        pid->state.prev_error = 0.0;
        pid->last_error = 0.0;
        pid->base.last_update_time = 0.0;
        LOG_INFO_MSG("PID 状态重置：ID=%u", pid->base.id);
    }
}
