/*
 * Copyright (c) 2026 Hollysys Co., Ltd.
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file logger.c
 * @brief 日志系统实现
 *
 * 实现多级别日志记录和文件轮转功能。
 */

#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>

// 全局日志状态
static struct {
    FILE* file;
    LogConfig config;
    pthread_mutex_t mutex;
    int initialized;
} g_logger = {NULL, {LOG_INFO, NULL, 10, 3}, PTHREAD_MUTEX_INITIALIZER, 0};

// 内部函数：检查并轮转日志文件
static void rotate_log_if_needed(void);

// 内部函数：获取文件大小（MB）
static size_t get_file_size_mb(const char* path);

int logger_init(const LogConfig* config) {
    if (!config || !config->file_path) {
        return -1;
    }

    pthread_mutex_lock(&g_logger.mutex);

    // 如果已初始化，先清理
    if (g_logger.initialized) {
        if (g_logger.file) {
            fclose(g_logger.file);
            g_logger.file = NULL;
        }
    }

    // 复制配置
    g_logger.config = *config;

    // 打开日志文件（追加模式）
    g_logger.file = fopen(config->file_path, "a");
    if (!g_logger.file) {
        pthread_mutex_unlock(&g_logger.mutex);
        return -1;
    }

    // 设置行缓冲
    setvbuf(g_logger.file, NULL, _IOLBF, 0);

    g_logger.initialized = 1;
    pthread_mutex_unlock(&g_logger.mutex);

    LOG_INFO_MSG("日志系统初始化完成：文件=%s, 级别=%s",
                 config->file_path, logger_level_name(config->level));

    return 0;
}

void logger_cleanup(void) {
    pthread_mutex_lock(&g_logger.mutex);

    if (g_logger.file) {
        LOG_INFO_MSG("日志系统关闭");
        fclose(g_logger.file);
        g_logger.file = NULL;
    }

    g_logger.initialized = 0;
    pthread_mutex_unlock(&g_logger.mutex);
}

void logger_log(LogLevel level, const char* file, int line, const char* format, ...) {
    if (!g_logger.initialized || !g_logger.file) {
        return;
    }

    // 检查日志级别
    if (level < g_logger.config.level) {
        return;
    }

    pthread_mutex_lock(&g_logger.mutex);

    // 检查文件大小并轮转
    rotate_log_if_needed();

    // 获取当前时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_buffer[64];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    // 提取文件名（去除路径）
    const char* filename = strrchr(file, '/');
    if (!filename) {
        filename = strrchr(file, '\\');
    }
    filename = filename ? filename + 1 : file;

    // 写入日志头部
    fprintf(g_logger.file, "[%s] [%s] [%s:%d] ",
            time_buffer, logger_level_name(level), filename, line);

    // 写入格式化消息
    va_list args;
    va_start(args, format);
    vfprintf(g_logger.file, format, args);
    va_end(args);

    fprintf(g_logger.file, "\n");

    pthread_mutex_unlock(&g_logger.mutex);
}

const char* logger_level_name(LogLevel level) {
    switch (level) {
        case LOG_DEBUG:   return "调试";
        case LOG_INFO:    return "信息";
        case LOG_WARNING: return "警告";
        case LOG_ERROR:   return "错误";
        default:          return "未知";
    }
}

static size_t get_file_size_mb(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return (size_t)(st.st_size / (1024 * 1024));
    }
    return 0;
}

static void rotate_log_if_needed(void) {
    if (!g_logger.config.file_path) {
        return;
    }

    // 检查当前文件大小
    size_t current_size = get_file_size_mb(g_logger.config.file_path);
    if (current_size < g_logger.config.max_size_mb) {
        return;
    }

    // 关闭当前文件
    if (g_logger.file) {
        fclose(g_logger.file);
        g_logger.file = NULL;
    }

    // 轮转备份文件
    for (int i = g_logger.config.backup_count - 1; i > 0; i--) {
        char old_path[512];
        char new_path[512];
        snprintf(old_path, sizeof(old_path), "%s.%d", g_logger.config.file_path, i - 1);
        snprintf(new_path, sizeof(new_path), "%s.%d", g_logger.config.file_path, i);
        rename(old_path, new_path);  // 忽略错误
    }

    // 重命名当前文件为 .0
    char backup_path[512];
    snprintf(backup_path, sizeof(backup_path), "%s.0", g_logger.config.file_path);
    rename(g_logger.config.file_path, backup_path);

    // 重新打开日志文件
    g_logger.file = fopen(g_logger.config.file_path, "a");
    if (g_logger.file) {
        setvbuf(g_logger.file, NULL, _IOLBF, 0);
    }
}
