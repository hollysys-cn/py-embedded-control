/**
 * @file debug_server.c
 * @brief 远程调试服务器管理实现
 */

#include "debug_server.h"
#include "logger.h"
#include <Python.h>

int debug_server_start(DebugSession* session, PyEmbedContext* py_context) {
    if (!session || !py_context) {
        LOG_ERROR_MSG("无效参数：session=%p, py_context=%p",
                      (void*)session, (void*)py_context);
        return -1;
    }

    LOG_INFO_MSG("启动 debugpy 服务器: %s:%d", session->host, session->port);

    // 设置状态为连接中
    debug_session_set_state(session, DEBUG_STATE_CONNECTING);

    // 导入 debugpy 模块
    PyObject* debugpy_module = PyImport_ImportModule("debugpy");
    if (!debugpy_module) {
        const char* error = "无法导入 debugpy 模块，请安装: pip install debugpy";
        debug_server_handle_error(session, error);
        PyErr_Clear();
        return -1;
    }

    // 获取 listen() 函数
    PyObject* listen_func = PyObject_GetAttrString(debugpy_module, "listen");
    if (!listen_func || !PyCallable_Check(listen_func)) {
        Py_DECREF(debugpy_module);
        const char* error = "debugpy.listen() 函数不可用";
        debug_server_handle_error(session, error);
        return -1;
    }

    // 构造参数：(host, port)
    PyObject* args = PyTuple_New(1);
    PyObject* address_tuple = PyTuple_New(2);
    PyTuple_SetItem(address_tuple, 0, PyUnicode_FromString(session->host));
    PyTuple_SetItem(address_tuple, 1, PyLong_FromLong(session->port));
    PyTuple_SetItem(args, 0, address_tuple);

    // 调用 debugpy.listen()
    PyObject* result = PyObject_CallObject(listen_func, args);

    Py_DECREF(args);
    Py_DECREF(listen_func);
    Py_DECREF(debugpy_module);

    if (!result) {
        const char* error = "调用 debugpy.listen() 失败";
        debug_server_handle_error(session, error);
        PyErr_Print();
        return -1;
    }

    Py_DECREF(result);

    // 设置状态为已连接（非阻塞模式，debugpy.listen() 立即返回）
    debug_session_set_state(session, DEBUG_STATE_CONNECTED);

    LOG_INFO_MSG("debugpy 服务器已启动，等待调试器连接...");
    LOG_INFO_MSG("VS Code 配置: \"host\": \"%s\", \"port\": %d", session->host, session->port);

    return 0;
}

int debug_server_stop(DebugSession* session) {
    if (!session) {
        return -1;
    }

    LOG_INFO_MSG("停止调试服务器");

    // 重置会话状态
    debug_session_reset(session);

    return 0;
}

void debug_server_check_status(DebugSession* session) {
    if (!session) {
        return;
    }

    DebugState state = debug_session_get_state(session);

    // 检查连接超时
    if (state == DEBUG_STATE_CONNECTING && debug_session_is_timeout(session)) {
        LOG_WARNING_MSG("调试连接超时（%d 秒），控制逻辑继续运行", session->timeout_sec);
        debug_session_set_error(session, "连接超时");
    }

    // 对于错误状态，仅记录警告（不影响控制逻辑）
    if (state == DEBUG_STATE_ERROR) {
        LOG_DEBUG_MSG("调试会话处于错误状态，控制逻辑继续正常运行");
    }
}

void debug_server_handle_error(DebugSession* session, const char* error_msg) {
    if (!session || !error_msg) {
        return;
    }

    // 记录错误并设置状态
    debug_session_set_error(session, error_msg);

    // 强调：调试错误不影响控制逻辑
    LOG_WARNING_MSG("调试功能不可用，但控制逻辑将继续运行");
    LOG_INFO_MSG("提示：若需要调试功能，请确保已安装 debugpy: pip install debugpy");
}
