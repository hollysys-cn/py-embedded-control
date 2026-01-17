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
 * @file config_loader.h
 * @brief 配置文件加载器接口
 *
 * 支持从 YAML 配置文件加载运行时配置。
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "config.h"

/**
 * @brief 从 YAML 文件加载配置
 * @param file_path 配置文件路径
 * @param config 输出配置结构体
 * @return 0 成功，-1 失败
 */
int config_load_from_file(const char* file_path, RuntimeConfig* config);

/**
 * @brief 验证配置有效性
 * @param config 配置结构体
 * @return 0 有效，-1 无效
 */
int config_validate(const RuntimeConfig* config);

#endif // CONFIG_LOADER_H
