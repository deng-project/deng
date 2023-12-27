// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ErrorDefinitions.h - C++ error handling definitions headers
// author: Karl-Mihkel Ott


#ifndef ERROR_DEFINITIONS_H
#define ERROR_DEFINITIONS_H

#include <string>
#include <stdexcept>
#include <cassert>
#include <iostream>

#include "deng/Api.h"

#ifdef ERROR_DEFINITIONS_CPP
    #include <Windows.h>
#endif

// Debugging function type definition
typedef void (*PFNGLERRORCHECK)(const std::string&, const std::string&, const uint32_t);
#ifdef DENG_DEBUG
    #define LOG(x) std::cout << "[Log] " << x << std::endl
    #define MAT_LOG(mat, msg) mat.log(msg);
    #define WARNME(x) std::cout << "[Warning] " << x << std::endl

    // OpenGL error checking function, that is enabled during debug mode but disabled otherwise
	#define glErrorCheck(func_name) lglErrorCheck(func_name, __FILE__, __LINE__ - 1)
    #define DENG_ASSERT(_val) assert(_val)
#else
    #define LOG(x)
    #define MAT_LOG(mat, msg)
    #define WARNME(x)
    #define glErrorCheck(func_name)
    #define DENG_ASSERT(_val)
#endif

// Exception handling:
// Severity levels should be used accordingly
// CRITICAL: error is so severe, that the program must be terminated
// NON_CRITICAL: the error doesn't require termination, but some functionality may be altered
// WARNING: generic warning
enum ErrorSeverity { CRITICAL, CRITICAL_NON_FATAL, NON_CRITICAL, WARNING, INFO };

namespace DENG {
    void DENG_API DispatchErrorToStdErr(const std::string& _sExceptionType, const std::string& _sWhat, ErrorSeverity _eSeverity);
}

#define DISPATCH_ERROR_MESSAGE(exception, what, severity) DENG::DispatchErrorToStdErr(exception, what, severity);

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      