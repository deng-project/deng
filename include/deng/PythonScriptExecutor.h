// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: PythonScriptExecutor.h - Python scripting layer header
// author: Karl-Mihkel Ott

#ifndef PYTHON_SCRIPT_EXECUTOR_H
#define PYTHON_SCRIPT_EXECUTOR_H

#include <type_traits>
#ifdef PYTHON_SCRIPT_EXECUTOR_CPP
    #include <string>
    #include <locale>
    #include <vector>
    #include <locale>
    #include <codecvt>
    #include <variant>
    #include <cstdarg>
    #include <cstring>

    #include "das/Api.h"
    #include "das/Algorithm.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
#endif

#define PY_SSIZE_T_CLEAN
#define PY_CORE_BUILTIN
#include "Python.h"

namespace DENG {

    class DENG_API PythonScriptExecutor {
        private: 
            std::wstring m_exec_name;
            PyConfig m_config = {};

        private:
            std::string _MakePlatformPath();
            void _ExceptionCheck(PyStatus &_status);
            std::wstring _WidenString(const std::string &_str);

        public:
            PythonScriptExecutor(const std::string &_exec_name);
            ~PythonScriptExecutor();

            /**
             * NOTE: Executed python functions can take following parameters with following string conventions
             *  "s" - string value (expected type const char*)
             *  "d" - integer value (expected type int32_t)
             *  "f" - floating point value (expected type double)
             * NOTE: Valid template types are const char*, int32_t and double 
             **/
            template<typename T>
            T ExecuteFunction(const std::string &_module_name, const std::string &_function_name, const char *_args = "", ...) {
                std::vector<char> va_types(std::strlen(_args));
                std::vector<std::variant<const char*, int32_t, double>> va_values;
                va_values.reserve(std::strlen(_args));

                for(size_t i = 0; i < std::strlen(_args); i++) {
                    // check if invalid argument character was passed
                    DENG_ASSERT(_args[i] == 's' || _args[i] == 'd' || _args[i] == 'f');
                    va_types[i] = _args[i];
                }

                va_list arguments;
                va_start(arguments, _args);

                // write all va_arg values to va_values
                for(char c : va_types) {
                    switch(c) {
                        case 's':
                            va_values.push_back(va_arg(arguments, const char*));
                            break;

                        case 'd':
                            va_values.push_back(va_arg(arguments, int32_t));
                            break;

                        case 'f':
                            va_values.push_back(va_arg(arguments, double));
                            break;
                    }
                }
                
                T return_val;

                // create python tuple with arguments
                PyObject *pargs = PyTuple_New(va_values.size());
                for(size_t i = 0; i < va_values.size(); i++) {
                    PyObject *pval = nullptr;
                    switch(va_types[i]) {
                        case 's':
                            pval = PyUnicode_DecodeFSDefault(std::get<const char*>(va_values[i]));
                            break;

                        case 'd':
                            pval = PyLong_FromLong(static_cast<long>(std::get<int32_t>(va_values[i])));
                            break;

                        case 'f':
                            pval = PyFloat_FromDouble(std::get<double>(va_values[i]));
                            break;
                    }
                    PyTuple_SetItem(pargs, i, pval);
                }


                PyObject *pname = PyUnicode_DecodeFSDefault(_module_name.c_str());
                PyObject *pmodule = PyImport_Import(pname);

                // 1. module is found
                // 2. module is not found
                if(pmodule != NULL) {
                    PyObject *pfunc = PyObject_GetAttrString(pmodule, _function_name.c_str());

                    if(pfunc && PyCallable_Check(pfunc)) {
                        PyObject *val = PyObject_CallObject(pfunc, pargs);
                        Py_DECREF(pargs);

                        if(val != NULL) {
                            if(std::is_integral<T>::value) {
                                return_val = static_cast<T>(PyLong_AsLong(val));
                            } else if(std::is_floating_point<T>::value) {
                                return_val = static_cast<T>(PyFloat_AsDouble(val));
                            } else {
                                return_val = static_cast<T>(reinterpret_cast<size_t>(PyUnicode_AsUTF8(val)));
                            }

                            Py_DECREF(val);
                        } else {
                            Py_DECREF(pfunc);
                            Py_DECREF(pmodule);
                            PyErr_Print();
                            std::cerr << "Function call failed" << std::endl;
                            std::exit(1);
                        }
                    } else {
                        std::cerr << "Cannot call python function '" << _function_name << "' in module '" << _module_name << "'" << std::endl;
                        std::exit(1);
                    }
                } else {
                    PyErr_Print();
                    std::cerr << "Invalid module with name '" << _module_name << "'" << std::endl;
                    std::exit(1);
                }

                return return_val;
            }


            void ExecuteSubroutine(const std::string &_module_name, const std::string &_sr_name, const char *_args, ...);
    };
    
}


#endif
