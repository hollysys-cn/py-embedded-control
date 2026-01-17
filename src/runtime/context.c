/**
 * @file context.c
 * @brief 运行时上下文管理实现
 */

#include "context.h"
#include "config_loader.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

// 全局运行时上下文（单例）
static RuntimeContext g_runtime_context = {0};
static int g_context_initialized = 0;

RuntimeContext* runtime_context_get(void) {
    return &g_runtime_context;
}

int runtime_context_init(const char* config_file) {
    if (g_context_initialized) {
        LOG_WARNING_MSG("运行时上下文已初始化");
        return 0;
    }

    // 加载配置
    if (config_load_from_file(config_file, &g_runtime_context.config) != 0) {
        LOG_ERROR_MSG("配置加载失败：%s", config_file);
        return -1;
    }

    // 初始化日志系统
    if (logger_init(&g_runtime_context.config.log_config) != 0) {
        fprintf(stderr, "日志系统初始化失败\n");
        return -1;
    }

    LOG_INFO_MSG("运行时上下文初始化：配置文件=%s", config_file);

    // 初始化 Python 解释器
    if (py_embed_init() != 0) {
        LOG_ERROR_MSG("Python 解释器初始化失败");
        logger_cleanup();
        return -1;
    }

    // 加载用户脚本
    if (py_embed_load_script(g_runtime_context.config.script_path,
                             &g_runtime_context.py_context) != 0) {
        LOG_ERROR_MSG("用户脚本加载失败：%s", g_runtime_context.config.script_path);
        py_embed_cleanup();
        logger_cleanup();
        return -1;
    }

    g_runtime_context.running = 0;
    g_runtime_context.cycle_count = 0;
    g_context_initialized = 1;

    LOG_INFO_MSG("运行时上下文初始化完成");

    return 0;
}

void runtime_context_cleanup(void) {
    if (!g_context_initialized) {
        return;
    }

    LOG_INFO_MSG("清理运行时上下文");

    g_runtime_context.running = 0;

    // 清理 Python 解释器
    py_embed_cleanup();

    // 清理日志系统
    logger_cleanup();

    g_context_initialized = 0;
}
