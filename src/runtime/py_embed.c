/**
 * @file py_embed.c
 * @brief Python 解释器嵌入实现
 */

#include "py_embed.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int py_embed_init(void) {
    // 初始化 Python 解释器
    Py_Initialize();

    if (!Py_IsInitialized()) {
        LOG_ERROR_MSG("Python 解释器初始化失败");
        return -1;
    }

    LOG_INFO_MSG("Python 解释器初始化成功：版本 %s", Py_GetVersion());

    // 添加当前目录到模块搜索路径
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.insert(0, '.')");
    PyRun_SimpleString("sys.path.insert(0, './python')");
    PyRun_SimpleString("sys.path.insert(0, './src/python_bindings')");

    return 0;
}

void py_embed_cleanup(void) {
    if (Py_IsInitialized()) {
        LOG_INFO_MSG("关闭 Python 解释器");
        Py_Finalize();
    }
}

int py_embed_load_script(const char* script_path, PyEmbedContext* context) {
    if (!script_path || !context) {
        LOG_ERROR_MSG("无效参数：script_path=%p, context=%p",
                      (void*)script_path, (void*)context);
        return -1;
    }

    // 初始化上下文
    context->module = NULL;
    context->init_func = NULL;
    context->step_func = NULL;
    context->initialized = 0;

    // 提取模块名（去除 .py 扩展名和路径）
    char* script_path_copy = strdup(script_path);
    char* base_name = basename(script_path_copy);
    char module_name[256];
    strncpy(module_name, base_name, sizeof(module_name) - 1);

    // 去除 .py 扩展名
    char* dot = strrchr(module_name, '.');
    if (dot) {
        *dot = '\0';
    }

    LOG_INFO_MSG("加载 Python 脚本：%s（模块名：%s）", script_path, module_name);

    // 导入模块
    PyObject* py_module_name = PyUnicode_DecodeFSDefault(module_name);
    context->module = PyImport_Import(py_module_name);
    Py_DECREF(py_module_name);
    free(script_path_copy);

    if (!context->module) {
        LOG_ERROR_MSG("无法导入 Python 模块：%s", module_name);
        py_embed_handle_exception();
        return -1;
    }

    // 获取 init() 函数
    context->init_func = PyObject_GetAttrString(context->module, "init");
    if (!context->init_func || !PyCallable_Check(context->init_func)) {
        LOG_ERROR_MSG("脚本缺少 init() 函数或函数不可调用");
        Py_XDECREF(context->init_func);
        Py_DECREF(context->module);
        return -1;
    }

    // 获取 step() 函数
    context->step_func = PyObject_GetAttrString(context->module, "step");
    if (!context->step_func || !PyCallable_Check(context->step_func)) {
        LOG_ERROR_MSG("脚本缺少 step() 函数或函数不可调用");
        Py_DECREF(context->init_func);
        Py_XDECREF(context->step_func);
        Py_DECREF(context->module);
        return -1;
    }

    context->initialized = 1;
    LOG_INFO_MSG("Python 脚本加载成功");

    return 0;
}

int py_embed_call_init(PyEmbedContext* context) {
    if (!context || !context->initialized || !context->init_func) {
        LOG_ERROR_MSG("无效的 Python 上下文");
        return -1;
    }

    LOG_INFO_MSG("调用用户脚本 init() 函数");

    // 调用 init() 函数
    PyObject* result = PyObject_CallObject(context->init_func, NULL);

    if (!result) {
        LOG_ERROR_MSG("init() 函数执行失败");
        py_embed_handle_exception();
        return -1;
    }

    Py_DECREF(result);
    LOG_INFO_MSG("init() 函数执行成功");

    return 0;
}

int py_embed_call_step(PyEmbedContext* context) {
    if (!context || !context->initialized || !context->step_func) {
        return -1;
    }

    // 调用 step() 函数
    PyObject* result = PyObject_CallObject(context->step_func, NULL);

    if (!result) {
        LOG_ERROR_MSG("step() 函数执行失败");
        py_embed_handle_exception();
        return -1;
    }

    Py_DECREF(result);
    return 0;
}

void py_embed_handle_exception(void) {
    if (!PyErr_Occurred()) {
        return;
    }

    // 获取异常信息
    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

    // 转换异常为字符串
    if (pvalue) {
        PyObject* str_obj = PyObject_Str(pvalue);
        if (str_obj) {
            const char* error_msg = PyUnicode_AsUTF8(str_obj);
            if (error_msg) {
                LOG_ERROR_MSG("Python 异常：%s", error_msg);
            }
            Py_DECREF(str_obj);
        }
    }

    // 打印详细回溯（到 stderr）
    if (ptraceback) {
        PyObject* traceback_module = PyImport_ImportModule("traceback");
        if (traceback_module) {
            PyObject* format_tb_func = PyObject_GetAttrString(traceback_module, "format_tb");
            if (format_tb_func && PyCallable_Check(format_tb_func)) {
                PyObject* tb_list = PyObject_CallFunctionObjArgs(format_tb_func, ptraceback, NULL);
                if (tb_list && PyList_Check(tb_list)) {
                    Py_ssize_t size = PyList_Size(tb_list);
                    for (Py_ssize_t i = 0; i < size; i++) {
                        PyObject* line = PyList_GetItem(tb_list, i);
                        const char* line_str = PyUnicode_AsUTF8(line);
                        if (line_str) {
                            LOG_ERROR_MSG("  %s", line_str);
                        }
                    }
                    Py_DECREF(tb_list);
                }
                Py_XDECREF(format_tb_func);
            }
            Py_DECREF(traceback_module);
        }
    }

    // 清理异常对象
    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);

    // 清除异常状态
    PyErr_Clear();
}
