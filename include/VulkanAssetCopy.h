// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanAssetCopy.h - Asset BLOB to buffer copying helper class header for Vulkan
// author: Karl-Mihkel Ott

#ifndef VULKAN_ASSET_COPY_CPP
#define VULKAN_ASSET_COPY_CPP

#ifdef VULKAN_ASSET_COPY_CPP
    #include <vector>
    #include <string>
    #include <cstdint>
    #include <vulkan/vulkan.h>

    #include <nekowin/include/napi.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <VulkanAssets.h>
    #include <Vulkanresources.h>
#endif

namespace DENG {
    namespace Vulkan {

        class AssetCpy {
        private:
            void *m_udata;


        private:
            void _CopyVu2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void _CopyVm2D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void _CopyVu3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);
            void _CopyVm3D(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);

            void _BufferCopyVu2DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void _BufferCopyVm2DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void _BufferCopyVu3DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
            void _BufferCopyVm3DToVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset,
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);

        public:
            AssetCpy(void *udata);
            void CopyToBuffer(const VkDevice device, const das_Asset &asset, const VkDeviceMemory mem);

            // Merge in this context means that the src_buf is copied to dst_buf
            void BufferCpyVertBuffer(const VkDevice device, const VkCommandPool cmd_pool, const das_Asset &asset, 
                const VkBuffer src_buf, const VkBuffer dst_buf, const VkQueue g_queue);
        };
    }
}

#endif
