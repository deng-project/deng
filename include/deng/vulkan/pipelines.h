/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipelines.h - Vulkan pipeline manager class header
/// author: Karl-Mihkel Ott


#ifndef __VK_PIPELINES_H
#define __VK_PIPELINES_H

#ifdef __VK_PIPELINES_CPP
    #include <array>
    #include <vector>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <data/assets.h>
    #include <common/err_def.h>

    #include <deng/cross_api/shader_def.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/pipeline_ci_gen.h>
#endif


namespace deng {
    namespace vulkan {

        /// Convert given asset mode into its corresponding pipeline type
        deng_PipelineType assetModeToPipelineType(das_AssetMode am);
        
        class __vk_PipelineCreator {
        private:
            // Pipeline layouts for all pipeline types
            VkPipelineLayout m_vu2d_layout;
            VkPipelineLayout m_vm2d_layout;
            VkPipelineLayout m_vu3d_layout;
            VkPipelineLayout m_vm3d_layout;
            VkPipelineLayout m_ui_layout;

            // Descriptor set layouts, which are needed for 
            // pipeline layout creation
            VkDescriptorSetLayout &m_vu2d_ds_layout;
            VkDescriptorSetLayout &m_vm2d_ds_layout;
            VkDescriptorSetLayout &m_vu3d_ds_layout;
            VkDescriptorSetLayout &m_vm3d_ds_layout;
            VkDescriptorSetLayout &m_ui_ds_layout;
        
            std::array<__vk_PipelineData, PIPELINE_C> m_pipelines;

        private:
            /// Create a single pipeline layout
            void __mkPipelineLayout(VkDevice device, VkDescriptorSetLayout &ds_layout,
                                    VkPipelineLayout &pl_layout);

            /// Create new pipeline layouts for all compatible pipelines
            void __mkPipelineLayouts(VkDevice device);

        public:
            __vk_PipelineCreator (
                VkDescriptorSetLayout &vu2d, VkDescriptorSetLayout &vm2d,
                VkDescriptorSetLayout &vu3d, VkDescriptorSetLayout &vm3d,
                VkDescriptorSetLayout &ui
            );

            /// Create new pipelines 
            void mkPipelines(VkDevice device, VkExtent2D ext, VkRenderPass rp, 
                             VkSampleCountFlagBits sample_c, bool no_layout);


            void remkPipelines(VkDevice device, VkExtent2D ext, VkRenderPass rp, 
                               VkSampleCountFlagBits sample_c);

        // Getters / Setters
        public:
            __vk_PipelineData &getPipeline(deng_ui32_t id);
            std::array<__vk_PipelineData, PIPELINE_C> &getPipelines();
        };
    }
}

#endif
