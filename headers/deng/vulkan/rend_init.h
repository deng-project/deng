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


#ifndef __REND_INIT_H
#define __REND_INIT_H


#ifdef __REND_INIT_CPP
    #include <mutex>
    #include <vector>
    #include <array>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <utils/timer.h>
    #include <deng/camera.h>

    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/resources.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    
    #include <deng/vulkan/rend_infos.h>
    #include <deng/vulkan/ic.h>
    #include <deng/vulkan/scc.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/pipelines.h>
    #include <deng/vulkan/desc_set_layout.h>
    #include <deng/vulkan/desc_pool.h>
    #include <deng/vulkan/desc_sets.h>
    #include <deng/vulkan/dc.h>
    #include <deng/vulkan/tm.h>
    #include <deng/vulkan/ubm.h>
    #include <deng/vulkan/bm.h>
    #include <deng/vulkan/rm.h>
#endif


namespace deng {
    namespace vulkan {

        /// Vulkan renderer config variables
        struct __vk_ConfigVars {
            VkSampleCountFlagBits msaa_sample_count = VK_SAMPLE_COUNT_1_BIT;
            deng_bool_t enable_vsync = false;
            deng_bool_t enable_validation_layers = false;
            deng_bool_t gui_count_fps = false;
            deng_bool_t cli_count_fps = false;
            dengMath::vec4<deng_vec_t> background = {0.0f, 0.0f, 0.0f, 1.0f};
            deng::Camera3D *p_cam;
            deng::Window *p_win;
        };


        class __vk_RendererInitialiser {
        private:
            // All renderer initialisers
            __vk_InstanceCreator *m_p_ic;
            __vk_SwapChainCreator *m_p_scc;
            __vk_DescriptorSetsCreator *m_p_desc_c; 
            __vk_PipelineCreator *m_p_pl_c;
            __vk_ResourceManager *m_p_rm;
            __vk_DrawCaller *m_p_dc;

        protected:
            __GlobalRegistry &m_reg;
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_textures;

        public:
            __vk_RendererInitialiser (
                deng::Window &win, 
                const __vk_ConfigVars &conf,
                deng::__GlobalRegistry &reg,
                std::vector<deng_Id> &assets,
                std::vector<deng_Id> &textures
            );

            ~__vk_RendererInitialiser();

        /// Getters
        public:
            __vk_InstanceCreator &getIC();
            __vk_SwapChainCreator &getSCC();
            __vk_DescriptorSetsCreator &getDescC();
            __vk_PipelineCreator &getPipelineC();
            __vk_ResourceManager &getResMan();
            __vk_DrawCaller &getDrawCaller();
        };
    }
}

#endif