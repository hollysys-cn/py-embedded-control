/**
 * @file plcopen_module.c
 * @brief PLCopen Python C Extension 模块定义
 *
 * 此文件定义 Python 模块并注册所有功能块类型。
 */

#include <Python.h>
#include "../function_blocks/fb_pid.h"
#include "../function_blocks/fb_first_order.h"
#include "../runtime/logger.h"

// 外部声明：PID Python 类型（在 py_pid.c 中定义）
extern PyTypeObject PIDType;

// 外部声明：FirstOrder Python 类型（在 py_first_order.c 中定义）
extern PyTypeObject FirstOrderType;

// 模块文档字符串
PyDoc_STRVAR(module_doc,
"PLCopen 标准功能块 Python 接口\n\n"
"提供 PID 控制器、一阶惯性等功能块的 Python 绑定。\n"
"所有功能块遵循 IEC 61131-3 标准。\n");

// 模块方法（暂无模块级函数）
static PyMethodDef plcopen_methods[] = {
    {NULL, NULL, 0, NULL}  // 结束标记
};

// 模块定义
static struct PyModuleDef plcopen_module = {
    PyModuleDef_HEAD_INIT,
    "plcopen_c",          // 模块名
    module_doc,           // 模块文档
    -1,                   // 模块状态大小（-1 表示全局状态）
    plcopen_methods,      // 模块方法
};

// 模块初始化函数
PyMODINIT_FUNC PyInit_plcopen_c(void) {
    PyObject* module;

    // 准备 PID 类型
    if (PyType_Ready(&PIDType) < 0) {
        return NULL;
    }

    // 准备 FirstOrder 类型
    if (PyType_Ready(&FirstOrderType) < 0) {
        return NULL;
    }

    // 创建模块
    module = PyModule_Create(&plcopen_module);
    if (module == NULL) {
        return NULL;
    }

    // 添加 PID 类到模块
    Py_INCREF(&PIDType);
    if (PyModule_AddObject(module, "PID", (PyObject*)&PIDType) < 0) {
        Py_DECREF(&PIDType);
        Py_DECREF(module);
        return NULL;
    }

    // 添加 FirstOrder 类到模块
    Py_INCREF(&FirstOrderType);
    if (PyModule_AddObject(module, "FirstOrder", (PyObject*)&FirstOrderType) < 0) {
        Py_DECREF(&FirstOrderType);
        Py_DECREF(module);
        return NULL;
    }

    // 添加版本信息
    PyModule_AddStringConstant(module, "__version__", "0.1.0");

    return module;
}
