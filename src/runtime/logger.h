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
 * @file logger.h
 * @brief 日志系统接口定义
 *
 * 提供多级别日志记录功能，支持文件输出和日志轮转。
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

// 日志级别枚举
typedef enum {
    LOG_DEBUG = 0,      // 调试信息
    LOG_INFO = 1,       // 一般信息
    LOG_WARNING = 2,    // 警告信息
    LOG_ERROR = 3       // 错误信息
} LogLevel;

// 日志配置结构体
typedef struct {
    LogLevel level;         // 最低日志级别
    const char* file_path;  // 日志文件路径
    size_t max_size_mb;     // 最大文件大小（MB）
    int backup_count;       // 保留的备份文件数量
} LogConfig;

/**
 * @brief 初始化日志系统
 * @param config 日志配置
 * @return 0 成功，-1 失败
 */
int logger_init(const LogConfig* config);

/**
 * @brief 清理日志系统资源
 */
void logger_cleanup(void);

/**
 * @brief 记录日志消息
 * @param level 日志级别
 * @param file 源文件名
 * @param line 行号
 * @param format 格式化字符串
 * @param ... 可变参数
 */
void logger_log(LogLevel level, const char* file, int line, const char* format, ...);

/**
 * @brief 获取日志级别名称
 * @param level 日志级别
 * @return 级别名称字符串
 */
const char* logger_level_name(LogLevel level);

// 便捷宏定义
#define LOG_DEBUG_MSG(...)    logger_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO_MSG(...)     logger_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING_MSG(...)  logger_log(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR_MSG(...)    logger_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#endif // LOGGER_H
