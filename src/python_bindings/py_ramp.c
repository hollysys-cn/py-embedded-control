/**
 * @file py_ramp.c
 * @brief Ramp 功能块的 Python C 扩展绑定
 */

#include <Python.h>
#include "../function_blocks/fb_ramp.h"

/* Ramp 对象结构 */
typedef struct {
    PyObject_HEAD
    RampFB fb;
} RampObject;

/* Ramp.compute() */
static PyObject* Ramp_compute(RampObject* self, PyObject* args, PyObject* kwargs) {
    double input, dt;
    static char* kwlist[] = {"input", "dt", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", kwlist, &input, &dt)) {
        return NULL;
    }

    double output = ramp_compute(&self->fb, input, dt);
    return PyFloat_FromDouble(output);
}

/* Ramp.set_params() */
static PyObject* Ramp_set_params(RampObject* self, PyObject* args, PyObject* kwargs) {
    double rising_rate, falling_rate;
    static char* kwlist[] = {"rising_rate", "falling_rate", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", kwlist,
                                      &rising_rate, &falling_rate)) {
        return NULL;
    }

    if (ramp_set_params(&self->fb, rising_rate, falling_rate) != 0) {
        PyErr_SetString(PyExc_ValueError, "Invalid parameters");
        return NULL;
    }

    Py_RETURN_NONE;
}

/* Ramp.get_params() */
static PyObject* Ramp_get_params(RampObject* self, PyObject* Py_UNUSED(args)) {
    double rising_rate, falling_rate;
    ramp_get_params(&self->fb, &rising_rate, &falling_rate);

    PyObject* dict = PyDict_New();
    if (!dict) return NULL;

    PyDict_SetItemString(dict, "rising_rate", PyFloat_FromDouble(rising_rate));
    PyDict_SetItemString(dict, "falling_rate", PyFloat_FromDouble(falling_rate));

    return dict;
}

/* Ramp.reset() */
static PyObject* Ramp_reset(RampObject* self, PyObject* args) {
    double initial_value = 0.0;

    if (!PyArg_ParseTuple(args, "|d", &initial_value)) {
        return NULL;
    }

    ramp_reset(&self->fb, initial_value);
    Py_RETURN_NONE;
}

/* 方法表 */
static PyMethodDef Ramp_methods[] = {
    {"compute", (PyCFunction)Ramp_compute, METH_VARARGS | METH_KEYWORDS,
     "Compute ramp output with rate limiting"},
    {"set_params", (PyCFunction)Ramp_set_params, METH_VARARGS | METH_KEYWORDS,
     "Set ramp parameters"},
    {"get_params", (PyCFunction)Ramp_get_params, METH_NOARGS,
     "Get ramp parameters"},
    {"reset", (PyCFunction)Ramp_reset, METH_VARARGS,
     "Reset ramp state"},
    {NULL, NULL, 0, NULL}
};

/* Ramp.__init__() */
static int Ramp_init(RampObject* self, PyObject* args, PyObject* kwargs) {
    double rising_rate = 1.0;
    double falling_rate = 1.0;
    static char* kwlist[] = {"rising_rate", "falling_rate", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|dd", kwlist,
                                      &rising_rate, &falling_rate)) {
        return -1;
    }

    if (ramp_init(&self->fb, rising_rate, falling_rate) != 0) {
        PyErr_SetString(PyExc_ValueError, "Failed to initialize Ramp");
        return -1;
    }

    return 0;
}

/* Ramp.__new__() */
static PyObject* Ramp_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
    RampObject* self = (RampObject*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}

/* Ramp 类型对象 */
PyTypeObject RampType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "plcopen_c.Ramp",
    .tp_doc = "Ramp rate limiter function block",
    .tp_basicsize = sizeof(RampObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = Ramp_new,
    .tp_init = (initproc)Ramp_init,
    .tp_methods = Ramp_methods,
};
