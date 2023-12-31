// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Api.h - DENG_API macro definition header
// author: Karl-Mihkel Ott

#ifndef DENG_API_H
#define DENG_API_H

#if defined(_MSC_BUILD) && !defined(DENG_STATIC)
    #ifdef DENG_COMPLETE_EXPORT_LIBRARY
        #define DENG_API __declspec(dllexport)
    #else 
        #define DENG_API __declspec(dllimport)
    #endif
#else
    #define DENG_API
#endif

#ifdef _DEBUG
#define DENG_DEBUG
#endif

#endif
