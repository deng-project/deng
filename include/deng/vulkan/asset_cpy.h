/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: asset_cpy.cpp - asset BLOB to buffer copying helper class header for Vulkan
/// author: Karl-Mihkel Ott

#ifndef __VK_ASSET_CPY_H
#define __VK_ASSET_CPY_H

#ifdef __VK_ASSET_CPY_CPP
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/deng_math.h>

    #include <deng/vulkan/resources.h>
#endif

namespace deng {
    namespace vulkan {

        class __AssetCpy {
        private:
            void *m_udata;


        private:
            void __cpyVu2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void __cpyVm2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void __cpyVu3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void __cpyVm3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);

            void __bufCpyVu2DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void __bufCpyVm2DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void __bufCpyVu3DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void __bufCpyVm3DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);

        public:
            __AssetCpy(void *udata);
            void cpyToBuffer(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);

            // Merge in this context means that the src_buf is copied to dst_buf
            void bufCpyVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset, 
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
        };
    }
}

#endif
