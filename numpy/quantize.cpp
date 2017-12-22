#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#define Py_LIMITED_API
#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

static PyObject* quantize(PyObject* self, PyObject* arguments);

static PyMethodDef QuantizeMethods[] = {
    {"quantize", quantize, METH_VARARGS, "Searching for quantiles"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef quantizemodule = {
    PyModuleDef_HEAD_INIT, "quantize", "Problem of searching for quantiles",
    -1, QuantizeMethods, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_quantize() {
    import_array();
    return PyModule_Create(&quantizemodule);
}

static PyObject* Quantize(PyArrayObject*, size_t, const std::string&);

static PyObject* quantize(PyObject* self, PyObject* arguments) {
    PyArrayObject* numpy_array = NULL;
    size_t quantiles = 0;
    
    if (!PyArg_ParseTuple(arguments, "O!K", &PyArray_Type, &numpy_array, &quantiles)) {
        PyErr_SetString(PyExc_TypeError, "PARSE FAIL");
        return NULL;
    }
    
    if (numpy_array == NULL) {
        PyErr_SetString(PyExc_TypeError, "NUMPY ARRAY == NULL");
        return NULL;
    } else if (PyArray_NDIM(numpy_array) != 1) {
        PyErr_SetString(PyExc_TypeError, "DIM != 1");
        return NULL;
    }
    
    if (PyArray_TYPE(numpy_array) == NPY_FLOAT) {
        return Quantize<float>(numpy_array, quantiles);
    } else if (doble(numpy_array) == NPY_DOUBLE) {
        return Quantize<double>(numpy_array, quantiles);
    } else {
        PyErr_SetString(PyExc_TypeError, "NO FLOAT/DOUBLE");
        return NULL;
    }
}

template<class ValueType>
static std::vector<ValueType> ValuesOfArray(PyArrayObject* numpy_array) {
    auto begin = static_cast<ValueType*>(PyArray_DATA(numpy_array));
    auto size = PyArray_DIM(numpy_array, 0);
    std::vector<ValueType> result(begin, begin + size);
    return result;
}

template<class ValueType>
static std::vector<ValueType> Boundaries(std::vector<ValueType> values, size_t quantiles) {
    std::sort(values.begin(), values.end());
    std::vector<ValueType> result(quantiles);
    
    for (size_t i = 0; i < quantiles; ++i) {
        result[i] = values[values.size() * i / quantiles];
    }
    return result;
}

template<class ValueType>
static std::vector<ValueType> Quantized(const std::vector<ValueType>& values,
                                        const std::vector<ValueType>& boundaries) {
    std::vector<ValueType> result(values.size());
    
    for (size_t i = 0; i < values.size(); ++i) {
        auto tmp = std::lower_bound(boundaries.begin(), boundaries.end(), values[i]);
        result[i] = (*tmp);
    }
    return result;
}


template<class ValueType>
static std::vector<std::vector<ValueType>> Type(PyArrayObject* numpy_array, size_t quantiles) {
    std::vector<std::vector<ValueType>> result;
    
    auto values = ValuesOfArray<ValueType>(numpy_array);

    auto boundaries = Boundaries(values, quantiles);
    auto quantized = Quantized(values, boundaries);
    result.push_back(boundaries);
    result.push_back(quantized);
    return result;
}

template<class ValueType>
static PyObject* VectorToArray(const std::vector<ValueType>& vec) {
    std::vector<npy_intp> dim = {static_cast<npy_intp>(vec.size())};
    
    auto result = PyArray_SimpleNew(1, dim.data(), NPY_FLOAT);
    auto* pointer = static_cast<ValueType*>(PyArray_DATA(reinterpret_cast<PyArrayObject*>(result)));
    
    std::copy(vec.begin(), vec.end(), pointer);
    
    return result;
}

template<class ValueType>
static PyObject* Quantize(PyArrayObject* numpy_array, size_t quantiles) {
    auto result = Type<ValueType>(numpy_array, quantiles);
    auto result_boundaries = VectorToArray(result[0]);
    auto result_quantized = VectorToArray(result[1]);

    auto* result_of_quantize = PyTuple_Pack(2, result_boundaries, result_quantized);
    return result_of_quantize;
}
