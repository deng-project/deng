// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.h - Vulkan renderer class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H


// tmp
#ifndef VULKAN_RENDERER_CPP
#define VULKAN_RENDERER_CPP
#endif

#ifdef VULKAN_RENDERER_CPP
    #include <string>
    #include <vector>
    #include <cstring>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>

    #define DEFAULT_VERTICES_SIZE   2048
    #define DEFAULT_INDICES_SIZE    2048
#endif

#include <VulkanHelpers.h>
#include <VulkanInstanceCreator.h>
#include <VulkanSwapchainCreator.h>
#include <VulkanPipelineCreator.h>
#include <VulkanDescriptorPoolCreator.h>
#include <VulkanDescriptorSetLayoutCreator.h>
#include <VulkanUniformBufferAllocator.h>
#include <VulkanDescriptorSetsCreator.h>



namespace DENG {

    class DENG_API VulkanRenderer : public Renderer {
        private:
            Vulkan::InstanceCreator *mp_instance_creator;
            Vulkan::SwapchainCreator *mp_swapchain_creator;
            Vulkan::PipelineCreator *mp_pipeline_creator;
            std::vector<Vulkan::DescriptorPoolCreator*> m_descriptor_pool_creators;
            std::vector<Vulkan::DescriptorSetsCreator*> m_descriptor_sets_creators;
            Vulkan::UniformBufferAllocator *mp_ubo_allocator;
            Vulkan::DescriptorSetLayoutCreator *mp_descriptor_set_layout_creator;
            Vulkan::DescriptorSetsCreator *mp_descriptor_sets_creator;
            
            // locally managed vulkan resources
            VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_2_BIT; // tmp
            VkDeviceSize m_vertices_size = DEFAULT_VERTICES_SIZE;
            VkDeviceSize m_indices_size = DEFAULT_INDICES_SIZE;
            VkBuffer m_main_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_main_memory = VK_NULL_HANDLE;

            VkCommandPool m_command_pool = VK_NULL_HANDLE;
            std::vector<VkSemaphore> m_render_finished_semaphores;
            std::vector<VkSemaphore> m_image_available_semaphores;
            std::vector<VkFence> m_flight_fences;
            std::vector<VkCommandBuffer> m_cmd_buffers;

            // framebuffers and command buffers
            VkImage m_color_image = VK_NULL_HANDLE;
            VkDeviceMemory m_color_image_memory = VK_NULL_HANDLE;
            VkImageView m_color_image_view = VK_NULL_HANDLE;

            VkImage m_depth_image = VK_NULL_HANDLE;
            VkDeviceMemory m_depth_image_memory = VK_NULL_HANDLE;
            VkImageView m_depth_image_view = VK_NULL_HANDLE;

            std::vector<VkFramebuffer> m_framebuffers;
            std::vector<VkCommandBuffer> m_command_buffers;
            uint32_t m_current_frame = 0;

        private:
            void _CreateCommandPool();
            void _CreateSemaphores();
            void _AllocateBufferResources();
            void _ReallocateBufferResources(VkDeviceSize _old_vert_size, VkDeviceSize _old_ind_size);
            void _CreateColorResources();
            void _CreateDepthResources();
            void _CreateFrameBuffers();
            void _AllocateCommandBuffers();
            void _RecordCommandBuffers();


        public:
            VulkanRenderer(const Window &_win);
            ~VulkanRenderer();

            virtual void LoadShaders() override;
            virtual void UpdateUniforms(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) override;
            virtual void UpdateVertexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void UpdateIndexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
        
#if 0
        /// Main renderer class for Vulkan API
        class Renderer : private RendererInitialiser,
                         public RuntimeUpdater 
        {
        private:
            void *m_udata;
            __vk_ConfigVars m_config;
            dengMath::vec2<deng_i32_t> m_prev_size;
            deng_bool_t m_is_init = false;
            deng_bool_t m_is_idle = true;

        private:

            /// Free and destroy all active Vulkan API specific instances
            void __cleanup();


            /// Additional cleanup methods
            void __cleanRendImgResources();
            void __cleanDrawCommands();
            void __cleanTextures();
            void __cleanAssets();
            void __cleanPipelines();
            void __cleanDescPools();
            void __cleanDescSetLayouts();
            void __freeBuffers();
            void __cleanSemaphores();
            void __cleanDevice();
            void __swapChainRecreationCheck();

            void __makeFrame();

        public:
            __vk_Renderer(__vk_ConfigVars &cnf, deng::Registry &reg,
                std::vector<deng_Id> &assets, std::vector<deng_Id> &textures);


            ~__vk_Renderer();

            /// Prepare assets for rendering
            void prepareAsset(deng_Id id);


            /// Prepare textures for binding them with assets
            void prepareTexture(deng_Id id);

            
            /// Update all currently available light sources
            void setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);

            
            /// Submit new draw call
            void makeFrame();


            /// Setup the renderer, create descriptor sets, allocate buffers
            /// and record command buffers
            void setup();


            /// Wait for any queue operation to finish
            /// This method needs to be called whenever any 
            /// command or data buffers need to be updated
            void idle();

        // Setter and getter methods
        public:
            void setUIDataPtr(__ImGuiData *p_data);
            const deng_bool_t isInit();
            const __vk_BufferData &getBufferData();
        };

#endif
}

#endif
