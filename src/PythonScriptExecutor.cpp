// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: PythonScriptingApi.cpp - Python scripting layer implementation
// author: Karl-Mihkel Ott

#define PYTHON_SCRIPT_EXECUTOR_CPP
#include <PythonScriptExecutor.h>

namespace DENG {

    PythonScriptExecutor::PythonScriptExecutor(const std::string &_exec_name) {
        // set PYTHONPATH environment to current working directory
        std::string wd = Libdas::Algorithm::RelativePathToAbsolute("");

#ifdef _WIN32
        // if using windows try to change all backslashes to normal slashes
        for (char& c : wd) {
            if (c == '\\')
                c = '/';
        }
#endif
        // NOTE: std::codecvt_utf8 is deprecated from C++17, string casting should be implemented into DENG
        m_exec_name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(_exec_name);
        std::wstring python_lib_path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(wd) + L"Python/Lib";
        std::wstring python_dll_path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(wd) + L"Python/DLLs";

        PyStatus status;
        PyConfig_InitPythonConfig(&m_config);
        // This embedded Python implementation should be isolated from system installation
        m_config.isolated = 1;
        
        // program name
        status = PyConfig_SetString(&m_config, &m_config.program_name, m_exec_name.data());
        _ExceptionCheck(status);

        // python module paths
        wchar_t delim;
#ifdef _WIN32
        delim = ';';
#else
        delim = ':';
#endif
        std::wstring path = python_lib_path + delim + python_dll_path;
        status = PyConfig_SetString(&m_config, &m_config.pythonpath_env, path.data());
        _ExceptionCheck(status);

        // sys.prefix where DENG installation is located at
        std::string prefix = Libdas::Algorithm::GetProgramPath();
        std::wstring wprefix = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(prefix);
        status = PyConfig_SetString(&m_config, &m_config.base_prefix, wprefix.data());
        _ExceptionCheck(status);
        Py_InitializeFromConfig(&m_config);

    }


    PythonScriptExecutor::~PythonScriptExecutor() {
        Py_FinalizeEx();
        PyConfig_Clear(&m_config);
    }

    std::string PythonScriptExecutor::_MakePlatformPath() {
        std::string py_dir = Libdas::Algorithm::RelativePathToAbsolute("./Python");

#ifdef _WIN32
        // if using windows try to change all backslashes to normal slashes
        for (char& c : py_dir) {
            if (c == '\\')
                c = '/';
        }
#endif

        return py_dir;
    }


    void PythonScriptExecutor::_ExceptionCheck(PyStatus &_status) {
        if (PyStatus_Exception(_status)) {
            PyConfig_Clear(&m_config);

            if (_status.func != NULL)
                std::cerr << _status.func << ": ";
            std::cerr << _status.err_msg << std::endl;
            std::exit(_status.exitcode);
        }
    }


    void PythonScriptExecutor::ExecuteSubroutine(const std::string &_module_name, const std::string &_sr_name, const char *_args, ...) {
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
            PyObject *pfunc = PyObject_GetAttrString(pmodule, _sr_name.c_str());

            if(pfunc && PyCallable_Check(pfunc)) {
                PyObject *val = PyObject_CallObject(pfunc, pargs);
                Py_DECREF(pargs);

                if(val != NULL) {
                    Py_DECREF(val);
                } else {
                    Py_DECREF(pfunc);
                    Py_DECREF(pmodule);
                    PyErr_Print();
                    std::cerr << "Function call failed" << std::endl;
                    std::exit(1);
                }
            } else {
                std::cerr << "Cannot call python function '" << _sr_name << "' in module '" << _module_name << "'" << std::endl;
                std::exit(1);
            }
        } else {
            PyErr_Print();
            std::cerr << "Invalid module with name '" << _module_name << "'" << std::endl;
            std::exit(1);
        }
    }
}
