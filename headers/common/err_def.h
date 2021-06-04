/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#ifndef ERR_DEF_H
#define ERR_DEF_H

// Console loggers
#include <iostream>

#ifdef __DEBUG
    #define LOG(x) std::cout << "LOG: " << x << std::endl
    #define WARNME(x) std::cout << "WARNING: " << x << std::endl
#else
    #define LOG(x)
    #define WARNME(x)
#endif


// Runtime errors
#define MEM_ERR(x)                          throw std::runtime_error(std::string("Failed to allocate memory: ") + x)
#define FILE_ERR(x)                         throw std::runtime_error(std::string("Failed to read file: ") + x)
#define DIR_ERR(dir)                        throw std::runtime_error(std::string("Failed to open directory: ") + dir)
#define MATH_ERR(x)                         throw std::runtime_error(std::string("Math error: ") + x)
#define FONT_ERR(x)                         throw std::runtime_error(std::string("Font rasterisation error: ") + x)
#define RUN_ERR(method, x)                  throw std::runtime_error(std::string(method) + ": " + std::string(x))
#define INVALID_ASSET_TEX(asset, uuid)      throw std::runtime_error(std::string("Invalid texture specified on asset: ") + asset + ", " + uuid) 
#define INVALID_ASSET(asset, uuid)          throw std::runtime_error(std::string("Invalid asset: ") + asset + ", " + uuid)
#define UNDEFINED_ASSET_MODE(asset, uuid)   throw std::runtime_error(std::string("Unknown asset mode for asset: ") + asset + ", " + uuid)


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