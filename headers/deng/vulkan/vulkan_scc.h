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


#ifndef __VULKAN_SCC_H
#define __VULKAN_SCC_H

#ifdef __VULKAN_SCC_CPP
    #include <vector>
    #include <array>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>   
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <das/assets.h>
    
    #include <math/deng_math.h>
    #include <deng/surface_window.h>
    #include <deng/window.h>
    #include <deng/vulkan/vulkan_sd.h>
    #include <deng/vulkan/vulkan_qm.h>
    #include <deng/vulkan/vulkan_resources.h>
    #include <deng/vulkan/vulkan_rend_infos.h>
#endif

namespace deng {
    namespace vulkan {
        /* 
         * Class for creating swapchains and swapchain image views 
         */
        class __vk_SwapChainCreator : private __vk_SwapChainInfo, private __vk_DeviceInfo {
        private:
            __vk_QueueManager m_qff;
            VkSampleCountFlagBits m_msaa_sample_c;
        
        private:
            void __mkSwapChainSettings();
            void __mkSwapChain (
                VkSurfaceKHR &surface, 
                deng_ui32_t g_queue_i, 
                deng_ui32_t p_queue_i
            );
            void __mkRenderPass();
            void __mkSCImageViews();
        
        public:
            __vk_SwapChainCreator (
                VkDevice device, 
                Window *p_win,
                VkPhysicalDevice gpu, 
                VkSurfaceKHR surface, 
                __vk_QueueManager qff,
                VkSampleCountFlagBits sample_c
            );
            ~__vk_SwapChainCreator();

            void SCCleanup();

            /* System that checks for window resizing should be made */
            void remkSwapChain (
                VkDevice device,
                Window *p_win,
                VkSurfaceKHR surface, 
                VkSurfaceFormatKHR s_format
            );

        public:
            VkRenderPass getRp();
            VkExtent2D getExt();
            VkSwapchainKHR getSC();
            VkFormat getSF();
            std::vector<VkImage> getSCImg();
            std::vector<VkImageView> getSCImgViews();
        };  
    }
}
#endif
