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
