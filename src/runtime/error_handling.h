/*
 * Copyright (c) 2026 Hollysys Co., Ltd.
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file error_handling.h
 * @brief 统一的错误处理宏定义
 *
 * 提供一致的空指针检查、参数验证和错误日志记录。
 */

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "logger.h"

/**
 * 检查指针是否为空，如果为空则记录错误并返回
 *
 * @param ptr 要检查的指针
 * @param name 参数名称（用于日志）
 * @param ret_val 返回值
 */
#define CHECK_NULL_PTR(ptr, name, ret_val) \
    do { \
        if ((ptr) == NULL) { \
            LOG_ERROR_MSG("%s: %s 参数为空", __func__, (name)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查两个指针
 */
#define CHECK_NULL_PTR2(ptr1, name1, ptr2, name2, ret_val) \
    do { \
        CHECK_NULL_PTR(ptr1, name1, ret_val); \
        CHECK_NULL_PTR(ptr2, name2, ret_val); \
    } while(0)

/**
 * 检查三个指针
 */
#define CHECK_NULL_PTR3(ptr1, name1, ptr2, name2, ptr3, name3, ret_val) \
    do { \
        CHECK_NULL_PTR(ptr1, name1, ret_val); \
        CHECK_NULL_PTR(ptr2, name2, ret_val); \
        CHECK_NULL_PTR(ptr3, name3, ret_val); \
    } while(0)

/**
 * 检查文件指针
 */
#define CHECK_FILE_PTR(fp, filename, ret_val) \
    do { \
        if ((fp) == NULL) { \
            LOG_ERROR_MSG("%s: 无法打开文件 %s", __func__, (filename)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查内存分配
 */
#define CHECK_MALLOC(ptr, size, ret_val) \
    do { \
        if ((ptr) == NULL) { \
            LOG_ERROR_MSG("%s: 内存分配失败 (大小: %zu 字节)", \
                          __func__, (size_t)(size)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查参数范围
 */
#define CHECK_RANGE(val, min, max, name, ret_val) \
    do { \
        if ((val) < (min) || (val) > (max)) { \
            LOG_ERROR_MSG("%s: %s=%f 超出范围 [%f, %f]", \
                          __func__, (name), (double)(val), \
                          (double)(min), (double)(max)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查正数参数
 */
#define CHECK_POSITIVE(val, name, ret_val) \
    do { \
        if ((val) <= 0) { \
            LOG_ERROR_MSG("%s: %s=%f 必须为正数", \
                          __func__, (name), (double)(val)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查非负参数
 */
#define CHECK_NON_NEGATIVE(val, name, ret_val) \
    do { \
        if ((val) < 0) { \
            LOG_ERROR_MSG("%s: %s=%f 不能为负数", \
                          __func__, (name), (double)(val)); \
            return (ret_val); \
        } \
    } while(0)

#endif /* ERROR_HANDLING_H */
