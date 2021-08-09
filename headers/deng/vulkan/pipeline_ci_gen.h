/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipeline_ci_gen.h - Vulkan pipeline createinfo generation class header
/// author: Karl-Mihkel Ott


#ifndef __VK_PIPELINE_CI_GEN_H
#define __VK_PIPELINE_CI_GEN_H

#ifdef __VK_PIPELINE_CI_GEN_CPP
    #include <vector>
    #include <array>
    #include <string>
    #include <queue>
    #include <stdexcept>
    #include <stddef.h>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>

    #include <data/assets.h>
    #include <math/deng_math.h>
    #include <deng/cross_api/shader_def.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/resources.h>
    #include <deng/window.h>

    #include <imgui-layer/imgui_entity.h>
#endif


namespace deng {
    namespace vulkan {

        /// Structure to specify all used flags in certain pipeline creation
        struct __vk_PipelineCreationFlags {
            VkPolygonMode polygon_mode              = VK_POLYGON_MODE_FILL;
            VkCullModeFlagBits cull_mode            = VK_CULL_MODE_NONE;
            VkFrontFace front_face                  = VK_FRONT_FACE_CLOCKWISE;
            VkPrimitiveTopology primitive_topology  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            deng_bool_t add_depth_stencil           = false;
            deng_bool_t add_color_blend             = true;
            deng_bool_t add_scissoring              = false;
            VkSampleCountFlagBits sample_c          = VK_SAMPLE_COUNT_1_BIT;
            deng_ui32_t subpass_index               = 0;
        };


        class __vk_PipelineCreateInfoGenerator : private __vk_PipelineCreateInfoSpecifiers {
        private:
            VkDevice m_device;
            VkExtent2D m_ext;
            VkRenderPass m_renderpass;
            __vk_PipelineData &m_pipeline_data;

        private:
            /// Read SPIR-V binary data from shader files
            char *__readBinShader(const char *file_name, size_t &size);


            /// Read appropriate vertex and fragment shaders according to 
            /// the pipeline mode
            void __readShaders(deng_PipelineType pt, char **p_vert, size_t &vert_c, 
                char **p_frag, size_t &frag_c);


            /// Create new shader module from SPIR-V binaries
            VkShaderModule __mkShaderModule(char *bin, size_t bin_c);


            /// Get binding description info in VkVertexInputBindingDescription instance
            std::vector<VkVertexInputBindingDescription> __getBindingDesc();


            /// Get attribute description info in VkVertexInputAttributeDescription instances
            std::vector<VkVertexInputAttributeDescription> __getAttributeDescs();
            
        public:
            __vk_PipelineCreateInfoGenerator(VkDevice device, VkExtent2D extent, VkRenderPass renderpass, 
                __vk_PipelineData &pd);
            ~__vk_PipelineCreateInfoGenerator();


            /// Make createinfo instance for graphics pipeline
            VkGraphicsPipelineCreateInfo mkGraphicsPipelineInfo(const __vk_PipelineCreationFlags &pl_flags);
        };
    }
}

#endif
