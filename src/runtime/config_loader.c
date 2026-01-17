/**
 * @file config_loader.c
 * @brief 配置文件加载器实现
 *
 * 简化版本：支持基本的 YAML 解析（手动实现，避免第三方依赖）。
 * 生产环境建议使用 libyaml 或 PyYAML 通过 Python 解析。
 */

#include "config_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 内部函数：去除字符串首尾空白
static char* trim(char* str);

// 内部函数：解析键值对
static int parse_key_value(const char* line, char* key, char* value);

// 内部函数：解析日志级别
static LogLevel parse_log_level(const char* str);

RuntimeConfig config_default(void) {
    RuntimeConfig config;

    // 运行时默认配置
    config.cycle_period_ms = 100;
    strcpy(config.script_path, "python/examples/pid_temperature.py");
    config.timeout_threshold_percent = 110;

    // 日志默认配置
    config.log_config.level = LOG_INFO;
    config.log_config.file_path = "runtime.log";
    config.log_config.max_size_mb = 10;
    config.log_config.backup_count = 3;

    // 调试默认配置
    config.debug_enabled = 0;
    strcpy(config.debug_host, "0.0.0.0");
    config.debug_port = 5678;
    config.debug_timeout = 5;

    // 性能默认配置
    config.cpu_affinity = -1;
    config.max_function_blocks = 32;

    return config;
}

int config_load_from_file(const char* file_path, RuntimeConfig* config) {
    if (!file_path || !config) {
        return -1;
    }

    // 从默认配置开始
    *config = config_default();

    // 打开配置文件
    FILE* file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "错误：无法打开配置文件：%s\n", file_path);
        return -1;
    }

    // 逐行解析（简化版 YAML 解析）
    char line[512];
    char key[128], value[384];
    char section[64] = "";

    while (fgets(line, sizeof(line), file)) {
        // 跳过注释和空行
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') {
            continue;
        }

        // 检测节（section）
        if (strstr(trimmed, ":") && !strstr(trimmed, "  ")) {
            // 顶层键（节名）
            sscanf(trimmed, "%63[^:]:", section);
            trim(section);
            continue;
        }

        // 解析键值对
        if (parse_key_value(trimmed, key, value) == 0) {
            // 根据节和键设置配置
            if (strcmp(section, "runtime") == 0) {
                if (strcmp(key, "cycle_period_ms") == 0) {
                    config->cycle_period_ms = atoi(value);
                } else if (strcmp(key, "script_path") == 0) {
                    strncpy(config->script_path, value, sizeof(config->script_path) - 1);
                } else if (strcmp(key, "timeout_threshold_percent") == 0) {
                    config->timeout_threshold_percent = atoi(value);
                }
            } else if (strcmp(section, "logging") == 0) {
                if (strcmp(key, "level") == 0) {
                    config->log_config.level = parse_log_level(value);
                } else if (strcmp(key, "file") == 0) {
                    config->log_config.file_path = strdup(value);
                } else if (strcmp(key, "max_size_mb") == 0) {
                    config->log_config.max_size_mb = (size_t)atoi(value);
                } else if (strcmp(key, "backup_count") == 0) {
                    config->log_config.backup_count = atoi(value);
                }
            } else if (strcmp(section, "debug") == 0) {
                if (strcmp(key, "enabled") == 0) {
                    config->debug_enabled = (strcmp(value, "true") == 0);
                } else if (strcmp(key, "host") == 0) {
                    strncpy(config->debug_host, value, sizeof(config->debug_host) - 1);
                } else if (strcmp(key, "port") == 0) {
                    config->debug_port = atoi(value);
                } else if (strcmp(key, "timeout") == 0) {
                    config->debug_timeout = atoi(value);
                }
            } else if (strcmp(section, "performance") == 0) {
                if (strcmp(key, "cpu_affinity") == 0) {
                    config->cpu_affinity = atoi(value);
                } else if (strcmp(key, "max_function_blocks") == 0) {
                    config->max_function_blocks = atoi(value);
                }
            }
        }
    }

    fclose(file);
    return config_validate(config);
}

int config_validate(const RuntimeConfig* config) {
    if (!config) {
        return -1;
    }

    // 验证周期时间
    if (config->cycle_period_ms < 10 || config->cycle_period_ms > 1000) {
        fprintf(stderr, "错误：周期时间必须在 10-1000 毫秒范围内\n");
        return -1;
    }

    // 验证脚本路径
    if (strlen(config->script_path) == 0) {
        fprintf(stderr, "错误：脚本路径不能为空\n");
        return -1;
    }

    // 验证调试端口
    if (config->debug_enabled && (config->debug_port < 1024 || config->debug_port > 65535)) {
        fprintf(stderr, "错误：调试端口必须在 1024-65535 范围内\n");
        return -1;
    }

    return 0;
}

// 内部函数实现

static char* trim(char* str) {
    if (!str) return NULL;

    // 去除前导空白
    while (isspace((unsigned char)*str)) str++;

    // 空字符串
    if (*str == 0) return str;

    // 去除尾部空白
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    return str;
}

static int parse_key_value(const char* line, char* key, char* value) {
    // 查找冒号
    const char* colon = strchr(line, ':');
    if (!colon) {
        return -1;
    }

    // 提取键
    size_t key_len = colon - line;
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim(key);

    // 提取值
    strcpy(value, colon + 1);
    trim(value);

    return 0;
}

static LogLevel parse_log_level(const char* str) {
    if (strcmp(str, "DEBUG") == 0) return LOG_DEBUG;
    if (strcmp(str, "INFO") == 0) return LOG_INFO;
    if (strcmp(str, "WARNING") == 0) return LOG_WARNING;
    if (strcmp(str, "ERROR") == 0) return LOG_ERROR;
    return LOG_INFO;  // 默认
}
