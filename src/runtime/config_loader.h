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
