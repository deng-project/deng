/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipelines.cpp - Vulkan pipeline manager class implementation
/// author: Karl-Mihkel Ott


#define __VK_PIPELINES_CPP
#include <deng/vulkan/pipelines.h>


namespace deng {
    namespace vulkan {

        /// Convert given asset mode into its corresponding pipeline type
        deng_PipelineType assetModeToPipelineType(das_AssetMode am) {
            switch(am) {
                case DAS_ASSET_MODE_2D_UNMAPPED:            return DENG_PIPELINE_TYPE_UNMAPPED_2D;
                case DAS_ASSET_MODE_3D_UNMAPPED:            return DENG_PIPELINE_TYPE_UNMAPPED_3D;
                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:      return DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D;
                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:      return DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D;
                default:                                    return DENG_PIPELINE_TYPE_UNKNOWN;
            }
        }


        __vk_PipelineCreator::__vk_PipelineCreator (
            VkDescriptorSetLayout &vu2d, 
            VkDescriptorSetLayout &vm2d,
            VkDescriptorSetLayout &vu3d, 
            VkDescriptorSetLayout &vm3d,
            VkDescriptorSetLayout &ui
        ) : m_vu2d_ds_layout(vu2d), m_vm2d_ds_layout(vm2d), 
            m_vu3d_ds_layout(vu3d), m_vm3d_ds_layout(vm3d), m_ui_ds_layout(ui) {}


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
            __mkPipelineLayout(device, m_ui_ds_layout, m_ui_layout);
        }

        
        /// Create new pipelines 
        void __vk_PipelineCreator::mkPipelines (
            VkDevice device, 
            VkExtent2D ext,
            VkRenderPass rp, 
            VkSampleCountFlagBits sample_c,
            bool no_layout
        ) {
            // Check if new pipeline layouts should be crated
            if(!no_layout) __mkPipelineLayouts(device);

            // Specify the pipiline type and layout
            m_pipelines[UM2D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_2D;
            m_pipelines[TM2D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D;
            m_pipelines[UM3D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_3D;
            m_pipelines[TM3D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D;
            m_pipelines[UI_I].pipeline_type = DENG_PIPELINE_TYPE_UI;

            m_pipelines[UM2D_I].p_pipeline_layout = &m_vu2d_layout;
            m_pipelines[TM2D_I].p_pipeline_layout = &m_vm2d_layout;
            m_pipelines[UM3D_I].p_pipeline_layout = &m_vu3d_layout;
            m_pipelines[TM3D_I].p_pipeline_layout = &m_vm3d_layout;
            m_pipelines[UI_I].p_pipeline_layout = &m_ui_layout;

            // Create pipeline creator instances
            __vk_PipelineCreateInfoGenerator vu2d_ci_gen(device, ext, rp, m_pipelines[UM2D_I]);
            __vk_PipelineCreateInfoGenerator vm2d_ci_gen(device, ext, rp, m_pipelines[TM2D_I]);
            __vk_PipelineCreateInfoGenerator vu3d_ci_gen(device, ext, rp, m_pipelines[UM3D_I]);
            __vk_PipelineCreateInfoGenerator vm3d_ci_gen(device, ext, rp, m_pipelines[TM3D_I]);
            __vk_PipelineCreateInfoGenerator ui_ci_gen(device, ext, rp, m_pipelines[UI_I]);

            // Setup the pipeline creation flags structure
            __vk_PipelineCreationFlags pi_flags = __vk_PipelineCreationFlags();
            pi_flags.add_color_blend = true;
            pi_flags.add_depth_stencil = false;
            pi_flags.add_scissoring = false;
            pi_flags.cull_mode = VK_CULL_MODE_NONE;
            pi_flags.polygon_mode = VK_POLYGON_MODE_FILL;
            pi_flags.front_face = VK_FRONT_FACE_CLOCKWISE;
            pi_flags.primitive_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            pi_flags.sample_c = sample_c;
            pi_flags.subpass_index = 0;

            // Generate all graphics pipeline createinfos
            std::array<VkGraphicsPipelineCreateInfo, PIPELINE_C> pipeline_infos{};
            pipeline_infos[UM2D_I] = vu2d_ci_gen.mkGraphicsPipelineInfo(pi_flags);
            pipeline_infos[TM2D_I] = vm2d_ci_gen.mkGraphicsPipelineInfo(pi_flags);

            pi_flags.add_depth_stencil = true;
            pipeline_infos[UM3D_I] = vu3d_ci_gen.mkGraphicsPipelineInfo(pi_flags);
            pipeline_infos[TM3D_I] = vm3d_ci_gen.mkGraphicsPipelineInfo(pi_flags);

            pi_flags.add_depth_stencil = false;
            pi_flags.add_scissoring = true;
            pipeline_infos[UI_I] = ui_ci_gen.mkGraphicsPipelineInfo(pi_flags);

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


        void __vk_PipelineCreator::remkPipelines(VkDevice device, VkExtent2D ext, VkRenderPass rp, VkSampleCountFlagBits sample_c) {
            for(size_t i = 0; i < m_pipelines.size(); i++)
                vkDestroyPipeline(device, m_pipelines[i].pipeline, NULL);

            mkPipelines(device, ext, rp, sample_c, true);
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
