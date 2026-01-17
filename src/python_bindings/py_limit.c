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
 * @file py_limit.c
 * @brief Limit 功能块的 Python C 扩展绑定
 */

#include <Python.h>
#include "../function_blocks/fb_limit.h"

/* Limit 对象结构 */
typedef struct {
    PyObject_HEAD
    LimitFB fb;
} LimitObject;

/* Limit.compute() */
static PyObject* Limit_compute(LimitObject* self, PyObject* args, PyObject* kwargs) {
    double input;
    static char* kwlist[] = {"input", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "d", kwlist, &input)) {
        return NULL;
    }

    double output = limit_compute(&self->fb, input);
    return PyFloat_FromDouble(output);
}

/* Limit.set_params() */
static PyObject* Limit_set_params(LimitObject* self, PyObject* args, PyObject* kwargs) {
    double min_value, max_value;
    static char* kwlist[] = {"min_value", "max_value", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", kwlist,
                                      &min_value, &max_value)) {
        return NULL;
    }

    if (limit_set_params(&self->fb, min_value, max_value) != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid parameters (min > max)");
        return NULL;
    }

    Py_RETURN_NONE;
}

/* Limit.get_params() */
static PyObject* Limit_get_params(LimitObject* self, PyObject* Py_UNUSED(args)) {
    double min_value, max_value;
    limit_get_params(&self->fb, &min_value, &max_value);

    PyObject* dict = PyDict_New();
    if (!dict) return NULL;

    PyDict_SetItemString(dict, "min_value", PyFloat_FromDouble(min_value));
    PyDict_SetItemString(dict, "max_value", PyFloat_FromDouble(max_value));

    return dict;
}

/* 方法表 */
static PyMethodDef Limit_methods[] = {
    {"compute", (PyCFunction)Limit_compute, METH_VARARGS | METH_KEYWORDS,
     "Compute limited output"},
    {"set_params", (PyCFunction)Limit_set_params, METH_VARARGS | METH_KEYWORDS,
     "Set limit parameters"},
    {"get_params", (PyCFunction)Limit_get_params, METH_NOARGS,
     "Get limit parameters"},
    {NULL, NULL, 0, NULL}
};

/* Limit.__init__() */
static int Limit_init(LimitObject* self, PyObject* args, PyObject* kwargs) {
    double min_value = 0.0;
    double max_value = 100.0;
    static char* kwlist[] = {"min_value", "max_value", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|dd", kwlist,
                                      &min_value, &max_value)) {
        return -1;
    }

    if (limit_init(&self->fb, min_value, max_value) != 0) {
        PyErr_SetString(PyExc_ValueError, "Failed to initialize Limit (min > max)");
        return -1;
    }

    return 0;
}

/* Limit.__new__() */
static PyObject* Limit_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
    LimitObject* self = (LimitObject*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}

/* Limit 类型对象 */
PyTypeObject LimitType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "plcopen_c.Limit",
    .tp_doc = "Limit clamping function block",
    .tp_basicsize = sizeof(LimitObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Limit_new,
    .tp_init = (initproc)Limit_init,
    .tp_methods = Limit_methods,
};
