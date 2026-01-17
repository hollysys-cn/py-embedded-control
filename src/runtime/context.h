/**
 * @file context.h
 * @brief 运行时上下文管理
 *
 * 管理全局运行时状态，包括配置、Python 解释器和功能块列表。
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include "py_embed.h"

// 运行时上下文结构体
typedef struct {
    RuntimeConfig config;        // 运行时配置
    PyEmbedContext py_context;   // Python 上下文
    int running;                 // 运行状态标志
    uint64_t cycle_count;        // 周期计数
} RuntimeContext;

/**
 * @brief 获取全局运行时上下文（单例）
 * @return 运行时上下文指针
 */
RuntimeContext* runtime_context_get(void);

/**
 * @brief 初始化运行时上下文
 * @param config_file 配置文件路径
 * @return 0 成功，-1 失败
 */
int runtime_context_init(const char* config_file);

/**
 * @brief 清理运行时上下文
 */
void runtime_context_cleanup(void);

#endif // CONTEXT_H
