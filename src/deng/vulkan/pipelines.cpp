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


#define __PIPELINES_CPP
#include <deng/vulkan/pipelines.h>


namespace deng {
    namespace vulkan {

        __vk_PipelineCreator::__vk_PipelineCreator (
            VkDescriptorSetLayout &vu2d, VkDescriptorSetLayout &vm2d,
            VkDescriptorSetLayout &vu3d, VkDescriptorSetLayout &vm3d
        ) : m_vu2d_ds_layout(vu2d), m_vm2d_ds_layout(vm2d), 
            m_vu3d_ds_layout(vu3d), m_vm3d_ds_layout(vm3d) {}


        /// Create a single pipeline layout
        void __vk_PipelineCreator::__mkPipelineLayout (
            VkDevice device, 
            VkDescriptorSetLayout &ds_layout,
            VkPipelineLayout &pl_layout
        ) {
            VkPipelineLayoutCreateInfo pl_layout_info{};       
            pl_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pl_layout_info.pushConstantRangeCount = 0;
            pl_layout_info.setLayoutCount = 1;
            pl_layout_info.pSetLayouts = &ds_layout;

            if(vkCreatePipelineLayout(device, &pl_layout_info, NULL,
               &pl_layout) != VK_SUCCESS)
                VK_DESC_ERR("failed to create a pipeline layout");
        }


        /// Create new pipeline layouts for all compatible pipelines
        void __vk_PipelineCreator::__mkPipelineLayouts(VkDevice device) {
            __mkPipelineLayout(device, m_vu2d_ds_layout, m_vu2d_layout);
            __mkPipelineLayout(device, m_vm2d_ds_layout, m_vm2d_layout);
            __mkPipelineLayout(device, m_vu3d_ds_layout, m_vu3d_layout);
            __mkPipelineLayout(device, m_vm3d_ds_layout, m_vm3d_layout);
        }

        
        /// Create new pipelines 
        void __vk_PipelineCreator::mkPipelines (
            VkDevice device, 
            VkExtent2D ext,
            VkRenderPass rp, 
            VkSampleCountFlagBits sample_c
        ) {
            // Create new pipeline layouts
            __mkPipelineLayouts(device);

            // Specify the pipiline type and layout
            m_pipelines[UM2D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_2D;
            m_pipelines[TM2D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D;
            m_pipelines[UM3D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_3D;
            m_pipelines[TM3D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D;

            m_pipelines[UM2D_I].p_pipeline_layout = &m_vu2d_layout;
            m_pipelines[TM2D_I].p_pipeline_layout = &m_vm2d_layout;
            m_pipelines[UM3D_I].p_pipeline_layout = &m_vu3d_layout;
            m_pipelines[TM3D_I].p_pipeline_layout = &m_vm3d_layout;

            // Create pipeline creator instances
            __vk_PipelineCreateInfoGenerator vu2d_ci_gen(device, ext, 
                rp, m_pipelines[UM2D_I]);

            __vk_PipelineCreateInfoGenerator vm2d_ci_gen(device, ext, 
                rp, m_pipelines[TM2D_I]);

            __vk_PipelineCreateInfoGenerator vu3d_ci_gen(device, ext, 
                rp, m_pipelines[UM3D_I]);

            __vk_PipelineCreateInfoGenerator vm3d_ci_gen(device, ext, 
                rp, m_pipelines[TM3D_I]);


            // Generate all graphics pipeline createinfos
            // As of now no culling will be done
            std::array<VkGraphicsPipelineCreateInfo, PIPELINE_C> pipeline_infos{};
            pipeline_infos[UM2D_I] = vu2d_ci_gen.mkGraphicsPipelineInfo(DENG_PIPELINE_TYPE_UNMAPPED_2D, 
                VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, sample_c, 0);

            pipeline_infos[TM2D_I] = vm2d_ci_gen.mkGraphicsPipelineInfo(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D, 
                VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, sample_c, 0);

            pipeline_infos[UM3D_I] = vu3d_ci_gen.mkGraphicsPipelineInfo(DENG_PIPELINE_TYPE_UNMAPPED_3D, 
                VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, sample_c, 0);

            pipeline_infos[TM3D_I] = vm3d_ci_gen.mkGraphicsPipelineInfo(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D, 
                VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, true, true, sample_c, 0);

            // Create all vulkan pipelines
            std::array<VkPipeline, PIPELINE_C> pipelines;
            if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, static_cast<deng_ui32_t>(pipeline_infos.size()), 
               pipeline_infos.data(), NULL, pipelines.data()) != VK_SUCCESS) 
                VK_DESC_ERR("failed to create graphics pipelines!");

            else {
                // Copy all the newly created pipelines to class pipeline instance
                for(size_t i = 0; i < pipelines.size(); i++)
                    m_pipelines[i].pipeline = pipelines[i];
            }
        }


        // Getters / Setters
        std::array<__vk_PipelineData, PIPELINE_C> &__vk_PipelineCreator::getPipelines() { 
            return m_pipelines; 
        }

        __vk_PipelineData &__vk_PipelineCreator::getPipeline(deng_ui32_t id) {
            return m_pipelines[id];
        }
    }
}