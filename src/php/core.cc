/*
  +----------------------------------------------------------------------+
  | Phpy                                                                 |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | license@swoole.com so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <rango@swoole.com>                             |
  | Copyright: 上海识沃网络科技有限公司                                       |
  +----------------------------------------------------------------------+
 */

#include "phpy.h"

#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "stubs/phpy_core_arginfo.h"

static zend_class_entry *PyCore_ce;
static PyObject *module_builtins = nullptr;
static PyObject *module_phpy = nullptr;
static std::unordered_map<const char *, PyObject *> builtin_functions;
static std::unordered_map<const char *, PyObject *> modules;
static std::unordered_map<PyObject *, void (*)(PyObject *)> zend_objects;
static long eval_code_id = 0;

using phpy::CallObject;
using phpy::php::arg_1;
using phpy::php::arg_2;

ZEND_METHOD(PyCore, import) {
    size_t l_module;
    char *module;
    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(module, l_module)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto iter = modules.find(module);
    PyObject *m;
    if (iter != modules.end()) {
        m = iter->second;
    } else {
        m = PyImport_ImportModule(module);
        if (m == NULL) {
            PyErr_Print();
            zend_throw_error(NULL, "PyCore: could not import module '%s'", module);
            return;
        }
        Py_INCREF(m);
        modules.emplace(module, m);
    }
    phpy::php::new_module(return_value, m);
}

ZEND_METHOD(PyCore, eval) {
    size_t l_input_code;
    char *input_code;
    zval *zglobal_params = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_STRING(input_code, l_input_code)
    Z_PARAM_OPTIONAL
    Z_PARAM_ARRAY_OR_NULL(zglobal_params)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    std::string module_name = "eval_code_" + std::to_string(eval_code_id++);
    PyObject *module = PyModule_New(module_name.c_str());
    if (module == NULL) {
        PyErr_Print();
        RETURN_FALSE;
    }

    // Borrowed reference
    PyObject *globals = PyModule_GetDict(module);

    if (!phpy::php::is_null(zglobal_params)) {
        auto pglobal_params = array2dict(zglobal_params);
        auto status = PyDict_Merge(globals, pglobal_params, 0);
        Py_DECREF(pglobal_params);
        if (status != 0) {
            PyErr_Print();
            Py_DECREF(module);
            RETURN_FALSE;
        }
    }

    PyObject *result = PyRun_StringFlags(input_code, Py_file_input, globals, NULL, NULL);
    if (result == NULL) {
        PyErr_Print();
        RETVAL_FALSE;
    } else {
        phpy::php::new_module(return_value, module);
        Py_DECREF(result);
    }
    Py_DECREF(module);
}

ZEND_METHOD(PyCore, iter) {
    auto pyobj = arg_1(INTERNAL_FUNCTION_PARAM_PASSTHRU, phpy_object_get_ce());
    CHECK_ARG(pyobj);
    auto iter = PyObject_GetIter(pyobj);
    if (iter == NULL) {
        return;
    }
    phpy::php::new_iter(return_value, iter);
}

ZEND_METHOD(PyCore, next) {
    auto iter = arg_1(INTERNAL_FUNCTION_PARAM_PASSTHRU, phpy_iter_get_ce());
    CHECK_ARG(iter);
    auto next = PyIter_Next(iter);
    if (next == NULL) {
        return;
    }
    py2php(next, return_value);
}

ZEND_METHOD(PyCore, int) {
    zval *zv;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(zv)
    ZEND_PARSE_PARAMETERS_END_EX(return );

    PyObject *pv = long2long(zv);
    phpy::php::new_object(return_value, pv);
    Py_DECREF(pv);
}

ZEND_METHOD(PyCore, float) {
    zval *zv;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(zv)
    ZEND_PARSE_PARAMETERS_END_EX(return );

    PyObject *pv = PyFloat_FromDouble(zval_get_double(zv));
    phpy::php::new_object(return_value, pv);
    Py_DECREF(pv);
}

ZEND_METHOD(PyCore, fn) {
    zval *zv;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(zv)
    ZEND_PARSE_PARAMETERS_END_EX(return );

    PyObject *pv = callable2py(zv);
    phpy::php::new_fn(return_value, pv);
    Py_DECREF(pv);
}

ZEND_METHOD(PyCore, scalar) {
    auto pyobj = arg_1(INTERNAL_FUNCTION_PARAM_PASSTHRU, phpy_object_get_ce());
    CHECK_ARG(pyobj);
    py2php(pyobj, return_value, true);
    Py_DECREF(pyobj);
}

int php_class_core_init(INIT_FUNC_ARGS) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "PyCore", class_PyCore_methods);
    PyCore_ce = zend_register_internal_class(&ce);

    return SUCCESS;
}

void php_class_init_all(INIT_FUNC_ARGS) {
    // All Python built-in functions are static methods of Core class
    php_class_core_init(INIT_FUNC_ARGS_PASSTHRU);
    // PyObject is the parent class of all other classes
    php_class_object_init(INIT_FUNC_ARGS_PASSTHRU);
    php_class_str_init(INIT_FUNC_ARGS_PASSTHRU);
    php_class_module_init(INIT_FUNC_ARGS_PASSTHRU);
    php_class_dict_init(INIT_FUNC_ARGS_PASSTHRU);
    // Sequence Types — list, tuple, range
    php_class_sequence_init(INIT_FUNC_ARGS_PASSTHRU);
    php_class_list_init(INIT_FUNC_ARGS_PASSTHRU);
    php_class_tuple_init(INIT_FUNC_ARGS_PASSTHRU);
    // Hash Set
    php_class_set_init(INIT_FUNC_ARGS_PASSTHRU);
    // Type
    php_class_type_init(INIT_FUNC_ARGS_PASSTHRU);
    // Callable
    php_class_fn_init(INIT_FUNC_ARGS_PASSTHRU);
    // Iter
    php_class_iter_init(INIT_FUNC_ARGS_PASSTHRU);
}

PyMODINIT_FUNC php_init_python_module(void) {
    return py_module_create(true);
}

PHP_MINIT_FUNCTION(phpy) {
    if (PyImport_AppendInittab("phpy", php_init_python_module) == -1) {
        zend_error(E_ERROR, "Error: failed to call PyImport_AppendInittab()");
        return FAILURE;
    }
    srand(time(NULL));
    Py_InitializeEx(0);
    module_phpy = PyImport_ImportModule("phpy");
    if (!module_phpy) {
        PyErr_Print();
        zend_error(E_ERROR, "Error: could not import module 'phpy'");
        return FAILURE;
    }
    module_builtins = PyImport_ImportModule("builtins");
    if (!module_builtins) {
        PyErr_Print();
        zend_error(E_ERROR, "Error: could not import module 'builtins'");
        return FAILURE;
    }
    php_class_init_all(INIT_FUNC_ARGS_PASSTHRU);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(phpy) {
    if (module_phpy) {
        Py_DECREF(module_phpy);
    }
    if (module_builtins) {
        Py_DECREF(module_builtins);
    }
    for (auto kv : builtin_functions) {
        Py_DECREF(kv.second);
    }
    for (auto kv : modules) {
        Py_DECREF(kv.second);
    }
    builtin_functions.clear();
    modules.clear();
    Py_Finalize();
    return SUCCESS;
}

namespace phpy {
namespace php {
void add_object(PyObject *pv, void (*dtor)(PyObject *)) {
    zend_objects.emplace(pv, dtor);
}
void del_object(PyObject *pv) {
    zend_objects.erase(pv);
}
}  // namespace php
}  // namespace phpy

PHP_RSHUTDOWN_FUNCTION(phpy) {
    for (auto kv : zend_objects) {
        kv.second(kv.first);
    }
    zend_objects.clear();
    return SUCCESS;
}

ZEND_METHOD(PyCore, __callStatic) {
    char *name;
    size_t l_name;
    zval *arguments;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STRING(name, l_name)
    Z_PARAM_ARRAY(arguments)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto fn_iter = builtin_functions.find(name);
    PyObject *fn;
    if (fn_iter == builtin_functions.end()) {
        fn = PyObject_GetAttrString(module_builtins, name);
        if (!fn || !PyCallable_Check(fn)) {
            PyErr_Print();
            zend_throw_error(NULL, "PyCore: has no builtin function '%s'", name);
            return;
        }
        builtin_functions[name] = fn;
    } else {
        fn = fn_iter->second;
    }

    CallObject caller(fn, return_value, arguments);
    caller.call();
}
