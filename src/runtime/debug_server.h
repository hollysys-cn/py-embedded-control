/**
 * @file debug_server.h
 * @brief 远程调试服务器管理
 *
 * 提供 debugpy 服务器的启动、停止和状态管理功能。
 * 调试功能是可选的，不影响控制逻辑的正常执行。
 */

#ifndef DEBUG_SERVER_H
#define DEBUG_SERVER_H

#include "debug_session.h"
#include "py_embed.h"

/**
 * @brief 启动调试服务器
 *
 * 通过 Python debugpy 模块启动远程调试服务器，允许 VS Code 等调试器连接。
 * 如果启动失败或连接超时，控制逻辑仍然继续运行。
 *
 * @param session 调试会话指针
 * @param py_context Python 嵌入上下文
 * @return 0 成功，-1 失败（失败不影响后续运行）
 */
int debug_server_start(DebugSession* session, PyEmbedContext* py_context);

/**
 * @brief 停止调试服务器
 *
 * @param session 调试会话指针
 * @return 0 成功，-1 失败
 */
int debug_server_stop(DebugSession* session);

/**
 * @brief 检查调试服务器状态
 *
 * 检查调试连接状态，如果连接超时则记录警告并将状态设置为错误。
 *
 * @param session 调试会话指针
 */
void debug_server_check_status(DebugSession* session);

/**
 * @brief 处理调试会话错误
 *
 * 确保调试错误不会影响控制逻辑的正常运行。
 *
 * @param session 调试会话指针
 * @param error_msg 错误消息
 */
void debug_server_handle_error(DebugSession* session, const char* error_msg);

#endif // DEBUG_SERVER_H
