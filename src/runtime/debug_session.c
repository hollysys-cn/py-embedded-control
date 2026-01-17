/**
 * @file debug_session.c
 * @brief 调试会话状态管理实现
 */

#include "debug_session.h"
#include "logger.h"
#include <string.h>
#include <stdio.h>

int debug_session_init(DebugSession* session, const char* host, int port, int timeout_sec) {
    if (!session || !host) {
        return -1;
    }

    memset(session, 0, sizeof(DebugSession));
    session->state = DEBUG_STATE_IDLE;
    strncpy(session->host, host, sizeof(session->host) - 1);
    session->port = port;
    session->timeout_sec = timeout_sec;
    session->error_count = 0;

    LOG_INFO_MSG("调试会话已初始化: %s:%d (超时: %d 秒)", host, port, timeout_sec);

    return 0;
}

void debug_session_set_state(DebugSession* session, DebugState state) {
    if (!session) {
        return;
    }

    DebugState old_state = session->state;
    session->state = state;

    // 记录状态转换
    LOG_DEBUG_MSG("调试会话状态转换: %s → %s",
                  debug_state_to_string(old_state),
                  debug_state_to_string(state));

    // 特殊状态处理
    switch (state) {
        case DEBUG_STATE_CONNECTING:
            session->connect_start_time = time(NULL);
            break;

        case DEBUG_STATE_CONNECTED:
            session->error_count = 0;
            memset(session->error_message, 0, sizeof(session->error_message));
            break;

        case DEBUG_STATE_ERROR:
            session->error_count++;
            break;

        case DEBUG_STATE_IDLE:
            session->connect_start_time = 0;
            break;

        default:
            break;
    }
}

DebugState debug_session_get_state(const DebugSession* session) {
    if (!session) {
        return DEBUG_STATE_ERROR;
    }
    return session->state;
}

void debug_session_set_error(DebugSession* session, const char* error_msg) {
    if (!session || !error_msg) {
        return;
    }

    debug_session_set_state(session, DEBUG_STATE_ERROR);
    strncpy(session->error_message, error_msg, sizeof(session->error_message) - 1);

    LOG_ERROR_MSG("调试会话错误: %s", error_msg);
}

int debug_session_is_timeout(const DebugSession* session) {
    if (!session || session->state != DEBUG_STATE_CONNECTING) {
        return 0;
    }

    time_t current_time = time(NULL);
    time_t elapsed = current_time - session->connect_start_time;

    if (elapsed >= session->timeout_sec) {
        LOG_WARNING_MSG("调试连接超时（%ld 秒 >= %d 秒）", elapsed, session->timeout_sec);
        return 1;
    }

    return 0;
}

void debug_session_reset(DebugSession* session) {
    if (!session) {
        return;
    }

    LOG_INFO_MSG("重置调试会话");

    session->state = DEBUG_STATE_IDLE;
    session->connect_start_time = 0;
    memset(session->error_message, 0, sizeof(session->error_message));
}

const char* debug_state_to_string(DebugState state) {
    switch (state) {
        case DEBUG_STATE_IDLE:
            return "IDLE";
        case DEBUG_STATE_CONNECTING:
            return "CONNECTING";
        case DEBUG_STATE_CONNECTED:
            return "CONNECTED";
        case DEBUG_STATE_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}
