// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ErrorDefinitions.h - C++ error handling definitions headers
// author: Karl-Mihkel Ott


#ifndef ERROR_DEFINITIONS_H
#define ERROR_DEFINITIONS_H

// Console loggers
#include <assert.h>
#include <iostream>

// Debugging function type definition
typedef void (*PFNGLERRORCHECK)(const std::string&, const std::string&, const uint32_t);
#ifdef _DEBUG
    #define LOG(x) std::cout << "LOG: " << x << std::endl
    #define MAT_LOG(mat, msg) mat.log(msg);
    #define WARNME(x) std::cout << "WARNING: " << x << std::endl

    // OpenGL error checking function, that is enabled during debug mode but disabled otherwise
	#define glErrorCheck(func_name) lglErrorCheck(func_name, __FILE__, __LINE__ - 1)
#else
    #define LOG(x)
    #define MAT_LOG(mat, msg);
    #define WARNME(x)
    #define glErrorCheck(func_name)
#endif


// Runtime errors
#define MEM_ERR(x)                          throw std::runtime_error(std::string("Failed to allocate memory: ") + x)
#define FILE_ERR(x)                         throw std::runtime_error(std::string("Failed to read file: ") + x)
#define DIR_ERR(dir)                        throw std::runtime_error(std::string("Failed to open directory: ") + dir)
#define MATH_ERR(x)                         throw std::runtime_error(std::string("Math error: ") + x)
#define FONT_ERR(x)                         throw std::runtime_error(std::string("Font rasterisation error: ") + x)
#define INVALID_ASSET_TEX(asset, uuid)      throw std::runtime_error(std::string("Invalid texture specified on asset: ") + asset + ", " + uuid) 
#define INVALID_ASSET(asset, uuid)          throw std::runtime_error(std::string("Invalid asset: ") + asset + ", " + uuid)
#define UNDEFINED_ASSET_MODE(asset, uuid)   throw std::runtime_error(std::string("Unknown asset mode for asset: ") + asset + ", " + uuid)
#define RUN_ERR(method, x)                  throw std::runtime_error(std::string(method) + ": " + std::string(x) + std::string("\n"))
#define DENG_ASSERT(msg, cond)              if(!(cond)) throw std::runtime_error(std::string(__FILE__) + std::string(", ") + std::to_string(__LINE__) + std::string(": ") + std::string(msg))


// Vulkan related
#define VK_GEN_ERR(x)                       throw std::runtime_error(std::string("Vulkan general error: ") + x)
#define VK_INSTANCE_ERR(x)                  throw std::runtime_error(std::string("Vulkan instance error: ") + x)
#define VK_SWAPCHAIN_ERR(x)                 throw std::runtime_error(std::string("Vulkan swapchain error: ") + x)
#define VK_DESC_ERR(x)                      throw std::runtime_error(std::string("Vulkan descriptor error: ") + x)
#define VK_RES_ERR(x)                       throw std::runtime_error(std::string("Vulkan resource allocation error: ") + x)
#define VK_DRAWCMD_ERR(x)                   throw std::runtime_error(std::string("Vulkan draw command error: ") + x)
#define VK_PIPELINEC_ERR(x)                 throw std::runtime_error(std::string("Vulkan pipeline creation error: ") + x)
#define VK_BUFFER_ERR(x)                    throw std::runtime_error(std::string("Vulkan buffer error: ") + x)
#define VK_FRAME_ERR(x)                     throw std::runtime_error(std::string("Vulkan frame error: ") + x)
#define VK_VAL_LAYER(x)                     std::cout << "VL: " << x << std::endl

#endif
