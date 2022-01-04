/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: deng.h - meta header file for all DENG functionality
/// author: Karl-Mihkel Ott


#ifndef __DENG_H
#define __DENG_H

// STL includes, that are necessary for some DENG components
#ifndef DENG_NO_STL_INCLUDES
    #include <vector>
    #include <queue>
    #include <array>
    #include <memory>
    #include <string>
    #include <cstring>
    #include <chrono>
#endif

// Third party dependencies
#include <vulkan/vulkan.h>

// DENG includes
#include <common/base_types.h>
#include <common/err_def.h>
#include <common/hashmap.h>
#include <data/assets.h>
#include <data/das_loader.h>
#include <data/tex_loader.h>
#include <math/deng_math.h>
#include <deng/window.h>
#include <deng/cam3d.h>
#include <deng/registry/registry.h>
#include <deng/renderer/renderer.h>

#endif