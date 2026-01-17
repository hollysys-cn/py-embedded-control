/**
 * @file config.h
 * @brief 运行时配置结构体定义
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "logger.h"

// 运行时配置结构体
typedef struct {
    // 运行时配置
    int cycle_period_ms;              // 控制周期（毫秒）
    char script_path[512];            // Python 脚本路径
    int timeout_threshold_percent;    // 超时阈值（周期的百分比）

    // 日志配置
    LogConfig log_config;

    // 调试配置
    int debug_enabled;                // 是否启用调试
    char debug_host[64];              // 调试监听地址
    int debug_port;                   // 调试端口
    int debug_timeout;                // 连接超时（秒）

    // 性能配置
    int cpu_affinity;                 // CPU 亲和性（-1 表示不绑定）
    int max_function_blocks;          // 最大功能块数量
} RuntimeConfig;

/**
 * @brief 创建默认配置
 * @return 默认配置结构体
 */
RuntimeConfig config_default(void);

#endif // CONFIG_H
