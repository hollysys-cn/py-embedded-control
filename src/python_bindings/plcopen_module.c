/**
 * @file plcopen_module.c
 * @brief PLCopen Python C Extension 模块定义
 */

#include <Python.h>
#include "../function_blocks/fb_pid.h"
#include "../function_blocks/fb_first_order.h"
#include "../function_blocks/fb_ramp.h"
#include "../function_blocks/fb_limit.h"
#include "../runtime/logger.h"

extern PyTypeObject PIDType;
extern PyTypeObject FirstOrderType;
extern PyTypeObject RampType;
extern PyTypeObject LimitType;

PyDoc_STRVAR(module_doc, "PLCopen function blocks");

static PyMethodDef plcopen_methods[] = {
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef plcopen_module = {
    PyModuleDef_HEAD_INIT,
    "plcopen_c",
    module_doc,
    -1,
    plcopen_methods,
};

PyMODINIT_FUNC PyInit_plcopen_c(void) {
    PyObject* module;

    if (PyType_Ready(&PIDType) < 0) return NULL;
    if (PyType_Ready(&FirstOrderType) < 0) return NULL;
    if (PyType_Ready(&RampType) < 0) return NULL;
    if (PyType_Ready(&LimitType) < 0) return NULL;

    module = PyModule_Create(&plcopen_module);
    if (module == NULL) return NULL;

    Py_INCREF(&PIDType);
    if (PyModule_AddObject(module, "PID", (PyObject*)&PIDType) < 0) {
        Py_DECREF(&PIDType);
        Py_DECREF(module);
        return NULL;
    }

    Py_INCREF(&FirstOrderType);
    if (PyModule_AddObject(module, "FirstOrder", (PyObject*)&FirstOrderType) < 0) {
        Py_DECREF(&FirstOrderType);
        Py_DECREF(module);
        return NULL;
    }

    Py_INCREF(&RampType);
    if (PyModule_AddObject(module, "Ramp", (PyObject*)&RampType) < 0) {
        Py_DECREF(&RampType);
        Py_DECREF(module);
        return NULL;
    }

    Py_INCREF(&LimitType);
    if (PyModule_AddObject(module, "Limit", (PyObject*)&LimitType) < 0) {
        Py_DECREF(&LimitType);
        Py_DECREF(module);
        return NULL;
    }

    PyModule_AddStringConstant(module, "__version__", "0.1.0");
    return module;
}
