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
 * @file debug_session.h
 * @brief 调试会话状态管理
 */

#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <time.h>

// 调试会话状态
typedef enum {
    DEBUG_STATE_IDLE,        // 空闲（未启用或未启动）
    DEBUG_STATE_CONNECTING,  // 正在连接
    DEBUG_STATE_CONNECTED,   // 已连接
    DEBUG_STATE_ERROR        // 错误状态
} DebugState;

// 调试会话结构体
typedef struct {
    DebugState state;           // 当前状态
    char host[64];              // 监听地址
    int port;                   // 监听端口
    int timeout_sec;            // 连接超时（秒）
    time_t connect_start_time;  // 连接开始时间
    char error_message[256];    // 错误信息
    int error_count;            // 错误计数
} DebugSession;

/**
 * @brief 初始化调试会话
 * @param session 调试会话指针
 * @param host 监听地址
 * @param port 监听端口
 * @param timeout_sec 连接超时（秒）
 * @return 0 成功，-1 失败
 */
int debug_session_init(DebugSession* session, const char* host, int port, int timeout_sec);

/**
 * @brief 设置会话状态
 * @param session 调试会话指针
 * @param state 新状态
 */
void debug_session_set_state(DebugSession* session, DebugState state);

/**
 * @brief 获取当前状态
 * @param session 调试会话指针
 * @return 当前状态
 */
DebugState debug_session_get_state(const DebugSession* session);

/**
 * @brief 记录错误信息
 * @param session 调试会话指针
 * @param error_msg 错误消息
 */
void debug_session_set_error(DebugSession* session, const char* error_msg);

/**
 * @brief 检查连接是否超时
 * @param session 调试会话指针
 * @return 1 超时，0 未超时
 */
int debug_session_is_timeout(const DebugSession* session);

/**
 * @brief 重置会话状态
 * @param session 调试会话指针
 */
void debug_session_reset(DebugSession* session);

/**
 * @brief 获取状态字符串
 * @param state 状态枚举值
 * @return 状态字符串
 */
const char* debug_state_to_string(DebugState state);

#endif // DEBUG_SESSION_H
