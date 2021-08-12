/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dc.h - Vulkan draw call wrapper class header
/// author: Karl-Mihkel Ott


#ifndef __VK_DC_H
#define __VK_DC_H


#ifdef __VK_DC_CPP
    #include <vector>
    #include <array>
    #include <queue>
	#include <string>
    
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <deng/cross_api/shader_def.h>
    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/resources.h>
    
    #include <deng/vulkan/rend_infos.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>

    #include <imgui-layer/imgui_entity.h>

    /// External frame count variable declaration
    extern deng_ui32_t __max_frame_c;
#endif


namespace deng {
    namespace vulkan {

        class __vk_DrawCaller {
        private:
            const std::vector<deng_Id> &m_assets;
            const std::vector<deng_Id> &m_textures;
            const std::vector<VkDescriptorSet> &m_ui_sets;
            deng::Registry &m_reg;
            std::vector<VkFramebuffer> m_framebuffers;
            std::array<__vk_PipelineData, PIPELINE_C> m_pl_data;
            const __vk_QueueManager &m_qff;

            // Commandpools and commandbuffers
            VkCommandPool m_cmd_pool;
            std::vector<VkCommandBuffer> m_cmd_bufs;

            // UI data pointer
            __ImGuiData *m_p_ui_data = NULL;

        private:
            void __mkSynchronisation(VkDevice &device);

            /// Bind asset resources to command buffers
            void __bindAssetResources(das_Asset &asset, VkCommandBuffer cur_buf,
                const __vk_BufferData &bd, const deng::BufferSectionInfo &buf_sec);


            /// Bind ImGui resources to command buffers
            void __bindUIElementResources(__ImGuiEntity *ent, VkCommandBuffer cur_buf, 
                const __vk_BufferData &bd, const deng::BufferSectionInfo &buf_sec);

            
            /// Bind asset pipeline and return its pipeline layout
            VkPipelineLayout *__bindAssetPipeline(das_Asset &asset, VkCommandBuffer cmd_buf);

        public:
            // Needed for synchronising frame creation with cpu
            deng_ui32_t current_frame = 0;
            std::vector<VkFence> flight_fences;
            std::vector<VkSemaphore> image_available_semaphore_set;
            std::vector<VkSemaphore> render_finished_semaphore_set;

        public:
            __vk_DrawCaller(VkDevice device, const __vk_QueueManager &qff,
                const std::vector<deng_Id> &assets, const std::vector<deng_Id> &textures,
                const std::vector<VkDescriptorSet> &ui_sets, deng::Registry &reg);
            
            void setMiscData(const std::array<__vk_PipelineData, PIPELINE_C> &pl_data, 
                const std::vector<VkFramebuffer> &fb);

            void mkCommandPool(VkDevice device);


            /// Allocate enough memory for command buffers
            void allocateCmdBuffers(VkDevice device, VkQueue g_queue, VkRenderPass renderpass, 
                VkExtent2D ext, const dengMath::vec4<deng_vec_t> &background, const __vk_BufferData &bd,
                const deng::BufferSectionInfo &buf_sec);


            /// Record command buffers for drawing assets and optionally ui elements
            void recordCmdBuffers(VkRenderPass renderpass, VkExtent2D ext,
                const dengMath::vec4<deng_vec_t> &background, const __vk_BufferData &bd,
                const deng::BufferSectionInfo &buf_sec);
        
        public:
            VkCommandPool getComPool();
            void setUIDataPtr(__ImGuiData *p_gui);
            const std::vector<VkCommandBuffer> &getComBufs();
        };
    }
}

#endif
