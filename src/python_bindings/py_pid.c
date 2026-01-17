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
 * @file py_pid.c
 * @brief PID 控制器 Python 绑定实现
 *
 * 将 C PID 功能块封装为 Python 类。
 */

#include <Python.h>
#include "../function_blocks/fb_pid.h"

// PID Python 对象结构
typedef struct {
    PyObject_HEAD
    PIDFunctionBlock* pid;  // C 功能块实例
} PIDObject;

// 析构函数
static void PID_dealloc(PIDObject* self) {
    if (self->pid) {
        pid_destroy(self->pid);
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

// 构造函数：__init__(self, Kp=1.0, Ki=0.0, Kd=0.0, output_min=-1e6, output_max=1e6)
static int PID_init(PIDObject* self, PyObject* args, PyObject* kwds) {
    double Kp = 1.0, Ki = 0.0, Kd = 0.0;
    double output_min = -1e6, output_max = 1e6;

    static char* kwlist[] = {"Kp", "Ki", "Kd", "output_min", "output_max", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ddddd", kwlist,
                                     &Kp, &Ki, &Kd, &output_min, &output_max)) {
        return -1;
    }

    self->pid = pid_create(Kp, Ki, Kd, output_min, output_max);
    if (!self->pid) {
        PyErr_SetString(PyExc_ValueError, "PID 创建失败：output_min 必须小于 output_max");
        return -1;
    }

    return 0;
}

// compute(SP, PV) -> float
static PyObject* PID_compute(PIDObject* self, PyObject* args) {
    double SP, PV;

    if (!PyArg_ParseTuple(args, "dd", &SP, &PV)) {
        return NULL;
    }

    if (!self->pid) {
        PyErr_SetString(PyExc_RuntimeError, "PID 实例未初始化");
        return NULL;
    }

    double output = pid_compute(self->pid, SP, PV, 0.0);
    return PyFloat_FromDouble(output);
}

// set_params(Kp=None, Ki=None, Kd=None)
static PyObject* PID_set_params(PIDObject* self, PyObject* args, PyObject* kwds) {
    PyObject* Kp_obj = NULL;
    PyObject* Ki_obj = NULL;
    PyObject* Kd_obj = NULL;

    static char* kwlist[] = {"Kp", "Ki", "Kd", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOO", kwlist,
                                     &Kp_obj, &Ki_obj, &Kd_obj)) {
        return NULL;
    }

    double Kp, Ki, Kd;
    double* Kp_ptr = NULL;
    double* Ki_ptr = NULL;
    double* Kd_ptr = NULL;

    if (Kp_obj && Kp_obj != Py_None) {
        Kp = PyFloat_AsDouble(Kp_obj);
        if (PyErr_Occurred()) return NULL;
        Kp_ptr = &Kp;
    }

    if (Ki_obj && Ki_obj != Py_None) {
        Ki = PyFloat_AsDouble(Ki_obj);
        if (PyErr_Occurred()) return NULL;
        Ki_ptr = &Ki;
    }

    if (Kd_obj && Kd_obj != Py_None) {
        Kd = PyFloat_AsDouble(Kd_obj);
        if (PyErr_Occurred()) return NULL;
        Kd_ptr = &Kd;
    }

    if (pid_set_params(self->pid, Kp_ptr, Ki_ptr, Kd_ptr) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "参数设置失败");
        return NULL;
    }

    Py_RETURN_NONE;
}

// get_params() -> dict
static PyObject* PID_get_params(PIDObject* self, PyObject* Py_UNUSED(ignored)) {
    const PIDParams* params = pid_get_params(self->pid);
    if (!params) {
        PyErr_SetString(PyExc_RuntimeError, "获取参数失败");
        return NULL;
    }

    PyObject* dict = PyDict_New();
    PyDict_SetItemString(dict, "Kp", PyFloat_FromDouble(params->Kp));
    PyDict_SetItemString(dict, "Ki", PyFloat_FromDouble(params->Ki));
    PyDict_SetItemString(dict, "Kd", PyFloat_FromDouble(params->Kd));
    PyDict_SetItemString(dict, "output_min", PyFloat_FromDouble(params->output_min));
    PyDict_SetItemString(dict, "output_max", PyFloat_FromDouble(params->output_max));

    return dict;
}

// get_state() -> dict
static PyObject* PID_get_state(PIDObject* self, PyObject* Py_UNUSED(ignored)) {
    const PIDState* state = pid_get_state(self->pid);
    if (!state) {
        PyErr_SetString(PyExc_RuntimeError, "获取状态失败");
        return NULL;
    }

    PyObject* dict = PyDict_New();
    PyDict_SetItemString(dict, "integral", PyFloat_FromDouble(state->integral));
    PyDict_SetItemString(dict, "prev_error", PyFloat_FromDouble(state->prev_error));
    PyDict_SetItemString(dict, "last_error", PyFloat_FromDouble(self->pid->last_error));

    return dict;
}

// reset()
static PyObject* PID_reset(PIDObject* self, PyObject* Py_UNUSED(ignored)) {
    pid_reset(self->pid);
    Py_RETURN_NONE;
}

// 方法表
static PyMethodDef PID_methods[] = {
    {"compute", (PyCFunction)PID_compute, METH_VARARGS,
     "计算 PID 控制输出\n\n参数:\n  SP: 设定值\n  PV: 过程变量/反馈值\n\n返回:\n  float: 控制变量"},
    {"set_params", (PyCFunction)PID_set_params, METH_VARARGS | METH_KEYWORDS,
     "动态修改 PID 参数\n\n参数:\n  Kp, Ki, Kd: 可选，仅修改提供的参数"},
    {"get_params", (PyCFunction)PID_get_params, METH_NOARGS,
     "获取当前 PID 参数\n\n返回:\n  dict: {Kp, Ki, Kd, output_min, output_max}"},
    {"get_state", (PyCFunction)PID_get_state, METH_NOARGS,
     "获取内部状态\n\n返回:\n  dict: {integral, prev_error, last_error}"},
    {"reset", (PyCFunction)PID_reset, METH_NOARGS,
     "重置 PID 内部状态（积分清零）"},
    {NULL, NULL, 0, NULL}
};

// 类型定义
PyTypeObject PIDType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "plcopen_c.PID",
    .tp_doc = "PID 控制器功能块\n\n标准位置式 PID 算法，支持输出限幅和抗积分饱和。",
    .tp_basicsize = sizeof(PIDObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)PID_init,
    .tp_dealloc = (destructor)PID_dealloc,
    .tp_methods = PID_methods,
};
