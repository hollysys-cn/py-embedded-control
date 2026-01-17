/**
 * @file py_first_order.c
 * @brief 一阶惯性功能块 Python 绑定实现
 */

#include <Python.h>
#include "../function_blocks/fb_first_order.h"

// FirstOrder Python 对象结构
typedef struct {
    PyObject_HEAD
    FirstOrderFunctionBlock* fo;
} FirstOrderObject;

// 析构函数
static void FirstOrder_dealloc(FirstOrderObject* self) {
    if (self->fo) {
        first_order_destroy(self->fo);
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

// 构造函数：__init__(self, T=1.0)
static int FirstOrder_init(FirstOrderObject* self, PyObject* args, PyObject* kwds) {
    double T = 1.0;

    static char* kwlist[] = {"T", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|d", kwlist, &T)) {
        return -1;
    }

    self->fo = first_order_create(T);
    if (!self->fo) {
        PyErr_SetString(PyExc_RuntimeError, "一阶惯性创建失败");
        return -1;
    }

    return 0;
}

// compute(input) -> float
static PyObject* FirstOrder_compute(FirstOrderObject* self, PyObject* args) {
    double input;

    if (!PyArg_ParseTuple(args, "d", &input)) {
        return NULL;
    }

    if (!self->fo) {
        PyErr_SetString(PyExc_RuntimeError, "实例未初始化");
        return NULL;
    }

    double output = first_order_compute(self->fo, input, 0.0);
    return PyFloat_FromDouble(output);
}

// set_time_constant(T)
static PyObject* FirstOrder_set_time_constant(FirstOrderObject* self, PyObject* args) {
    double T;

    if (!PyArg_ParseTuple(args, "d", &T)) {
        return NULL;
    }

    if (first_order_set_time_constant(self->fo, T) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "时间常数设置失败");
        return NULL;
    }

    Py_RETURN_NONE;
}

// get_params() -> dict
static PyObject* FirstOrder_get_params(FirstOrderObject* self, PyObject* Py_UNUSED(ignored)) {
    const FirstOrderParams* params = first_order_get_params(self->fo);
    if (!params) {
        PyErr_SetString(PyExc_RuntimeError, "获取参数失败");
        return NULL;
    }

    PyObject* dict = PyDict_New();
    PyDict_SetItemString(dict, "T", PyFloat_FromDouble(params->T));

    return dict;
}

// reset()
static PyObject* FirstOrder_reset(FirstOrderObject* self, PyObject* Py_UNUSED(ignored)) {
    first_order_reset(self->fo);
    Py_RETURN_NONE;
}

// 方法表
static PyMethodDef FirstOrder_methods[] = {
    {"compute", (PyCFunction)FirstOrder_compute, METH_VARARGS,
     "计算一阶惯性输出\n\n参数:\n  input: 输入信号\n\n返回:\n  float: 输出信号"},
    {"set_time_constant", (PyCFunction)FirstOrder_set_time_constant, METH_VARARGS,
     "动态修改时间常数\n\n参数:\n  T: 时间常数（秒）"},
    {"get_params", (PyCFunction)FirstOrder_get_params, METH_NOARGS,
     "获取参数\n\n返回:\n  dict: {T}"},
    {"reset", (PyCFunction)FirstOrder_reset, METH_NOARGS,
     "重置内部状态（输出清零）"},
    {NULL, NULL, 0, NULL}
};

// 类型定义
PyTypeObject FirstOrderType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "plcopen_c.FirstOrder",
    .tp_doc = "一阶惯性功能块\n\n传递函数：H(s) = 1 / (T*s + 1)",
    .tp_basicsize = sizeof(FirstOrderObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)FirstOrder_init,
    .tp_dealloc = (destructor)FirstOrder_dealloc,
    .tp_methods = FirstOrder_methods,
};
