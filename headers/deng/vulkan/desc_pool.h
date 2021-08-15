/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_pool.h - Vulkan descriptor pool creator class header
/// author: Karl-Mihkel Ott


#ifndef __VK_DESC_POOL_C_H
#define __VK_DESC_POOL_C_H


#ifdef __VK_DESC_POOL_C_CPP
    #include <array>
    #include <vector>
    #include <vulkan/vulkan.h>
    
    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
    #include <deng/window.h>
    #include <deng/lighting/light_srcs.h>
    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/registry/registry.h>
    #include <deng/vulkan/rend_infos.h>
    
    /// Maximum frame in flight count specifier
    extern deng_ui32_t __max_frame_c;
#endif


namespace deng {
    namespace vulkan {

        /// Descriptor pool reallocation result enum
        enum __vk_DescriptorPoolReallocResult {
            DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_NO_REALLOC   = 0x00,
            DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VU2D = 0x01,
            DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VM2D = 0x02,
            DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VU3D = 0x04,
            DENG_VK_DESCRIPTOR_POOL_REALLOC_RESULT_REALLOC_VM3D = 0x08
        };
        typedef uint8_t __vk_DescriptorPoolReallocBits;
        

        class __vk_DescriptorPoolCreator {
        private:
            // Needed for descriptor pool reallocations
            std::vector<deng_Id> &m_assets;
            deng::Registry &m_reg;

            // Unmapped 2D asset pool
            VkDescriptorPool m_vu2d_pool = {};
            deng_ui32_t m_vu2d_cap = 0;

            // Texture mapped 2D asset pool
            VkDescriptorPool m_vm2d_pool = {};
            deng_ui32_t m_vm2d_cap = 0;

            // Unmapped 3D asset pool
            VkDescriptorPool m_vu3d_pool = {};
            deng_ui32_t m_vu3d_cap = 0;

            // Texture mapped 3D asset pool
            VkDescriptorPool m_vm3d_pool = {};
            deng_ui32_t m_vm3d_cap = 0;

            // UI element pool
            VkDescriptorPool m_ui_pool = {};

            void *m_udata;

        private:
            void __mk2DUnmappedDescPool(VkDevice device);
            void __mk2DTexMappedDescPool(VkDevice device);
            void __mk3DUnmappedDescPool(VkDevice device);
            void __mk3DTexMappedDescPool(VkDevice device);
            void __mkUIPool(VkDevice device);

            /// Destroy assets with specific asset mode
            void __destroyAssetDescriptorSets(VkDevice device, 
                std::vector<deng_Id> &destroyed, das_AssetMode mode);

            /// Reallocate specific descriptor pool for specific asset type
            void __reallocDescPool(VkDevice device, std::vector<deng_Id> &destroyed_assets, 
                das_AssetMode asset_mode, deng_ui32_t req_c);

        protected:
            __vk_DescriptorPoolCreator(std::vector<deng_Id> &assets, Registry &reg, void *udata); 

            /// Create a new descriptor pool based on the pipeline type
            void mkDescPool(VkDevice device, deng_ui32_t cap, deng_PipelineType mode);

            /// Create new descriptor pools for all asset types
            void mkDescPools(VkDevice device, deng_ui32_t cap);

            /// Check if the descriptor pools need to be reallocated and reallocate
            /// the pool if needed
            __vk_DescriptorPoolReallocBits checkPoolReallocation(VkDevice device,
                std::vector<deng_Id> &destroyed_assets, deng_ui32_t vu2d_c, 
                deng_ui32_t vm2d_c, deng_ui32_t vu3d_c, deng_ui32_t vm3d_c);

        /// Getter / Setter methods
        public:
            VkDescriptorPool &getDescPool(deng_PipelineType type);
        };
    }
}

#endif
