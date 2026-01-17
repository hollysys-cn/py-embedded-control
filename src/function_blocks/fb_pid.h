/**
 * @file fb_pid.h
 * @brief PID 控制器功能块接口
 *
 * 标准位置式 PID 算法：CV = Kp*e + Ki*∫e + Kd*de/dt
 */

#ifndef FB_PID_H
#define FB_PID_H

#include "fb_common.h"

// PID 参数结构体
typedef struct {
    double Kp;          // 比例系数 [0, 1e6]
    double Ki;          // 积分系数 [0, 1e6]
    double Kd;          // 微分系数 [0, 1e6]
    double output_min;  // 输出下限
    double output_max;  // 输出上限
} PIDParams;

// PID 内部状态
typedef struct {
    double integral;    // 积分累积值
    double prev_error;  // 上一周期误差
} PIDState;

// PID 功能块完整结构
typedef struct {
    FunctionBlock base;  // 基础属性
    PIDParams params;    // 参数
    PIDState state;      // 状态
    double last_error;   // 当前误差（用于诊断）
} PIDFunctionBlock;

/**
 * @brief 创建 PID 功能块实例
 * @param Kp 比例系数
 * @param Ki 积分系数
 * @param Kd 微分系数
 * @param output_min 输出下限
 * @param output_max 输出上限
 * @return PID 功能块指针，失败返回 NULL
 */
PIDFunctionBlock* pid_create(double Kp, double Ki, double Kd,
                              double output_min, double output_max);

/**
 * @brief 销毁 PID 功能块实例
 * @param pid PID 功能块指针
 */
void pid_destroy(PIDFunctionBlock* pid);

/**
 * @brief 计算 PID 控制输出
 * @param pid PID 功能块指针
 * @param SP 设定值
 * @param PV 过程变量/反馈值
 * @param dt 时间步长（秒），0 表示自动计算
 * @return 控制变量（CV）
 */
double pid_compute(PIDFunctionBlock* pid, double SP, double PV, double dt);

/**
 * @brief 设置 PID 参数
 * @param pid PID 功能块指针
 * @param Kp 比例系数（NULL 表示不修改）
 * @param Ki 积分系数（NULL 表示不修改）
 * @param Kd 微分系数（NULL 表示不修改）
 * @return 0 成功，-1 失败
 */
int pid_set_params(PIDFunctionBlock* pid, const double* Kp,
                   const double* Ki, const double* Kd);

/**
 * @brief 获取 PID 参数
 * @param pid PID 功能块指针
 * @return 参数结构体指针
 */
const PIDParams* pid_get_params(const PIDFunctionBlock* pid);

/**
 * @brief 获取 PID 内部状态
 * @param pid PID 功能块指针
 * @return 状态结构体指针
 */
const PIDState* pid_get_state(const PIDFunctionBlock* pid);

/**
 * @brief 重置 PID 内部状态
 * @param pid PID 功能块指针
 */
void pid_reset(PIDFunctionBlock* pid);

#endif // FB_PID_H
