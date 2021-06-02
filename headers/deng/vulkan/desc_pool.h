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
 * ----------------------------------------------------------------
 *  Name: desc_pool_c - Descriptor pool creator for Vulkan renderer
 *  Purpose: Provide parent class for creating descriptor pools
 *  Author: Karl-Mihkel Ott
 */ 


#ifndef __DESC_POOL_C_H
#define __DESC_POOL_C_H


#ifdef __DESC_POOL_C_CPP
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
    #include <deng/lighting/light_srcs.h>
    #include <deng/vulkan/resources.h>
    #include <deng/registry/registry.h>
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
            deng::__GlobalRegistry &m_reg;

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

        private:
            void __mk2DUnmappedDescPool(VkDevice device);
            void __mk2DTexMappedDescPool(VkDevice device);
            void __mk3DUnmappedDescPool(VkDevice device);
            void __mk3DTexMappedDescPool(VkDevice device);

            /// Destroy assets with specific asset mode
            void __destroyAssetDescriptorSets(VkDevice device, 
                std::vector<deng_Id> &destroyed, das_AssetMode mode);

            /// Reallocate specific descriptor pool for specific asset type
            void __reallocDescPool(VkDevice device, std::vector<deng_Id> &destroyed_assets, 
                das_AssetMode asset_mode, deng_ui32_t req_c);

        protected:
            __vk_DescriptorPoolCreator(std::vector<deng_Id> &assets, 
                __GlobalRegistry &reg) : m_assets(assets), m_reg(reg) {}

            /// Create a new descriptor pool based on the asset mode
            void mkDescPool(VkDevice device, deng_ui32_t cap, das_AssetMode mode);

            /// Create new descriptor pools for all asset types
            void mkDescPools(VkDevice device, deng_ui32_t cap);

            /// Check if the descriptor pools need to be reallocated and reallocate
            /// the pool if needed
            __vk_DescriptorPoolReallocBits checkPoolReallocation(VkDevice device,
                std::vector<deng_Id> &destroyed_assets, deng_ui32_t vu2d_c, 
                deng_ui32_t vm2d_c, deng_ui32_t vu3d_c, deng_ui32_t vm3d_c);

        /// Getter / Setter methods
        public:
            VkDescriptorPool &getDescPool(das_AssetMode mode);
        };
    }
}

#endif
