// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.h - Vulkan renderer class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H


#ifdef VULKAN_RENDERER_CPP
    #include <string>
    #include <vector>
    #include <array>
    #include <thread>
    #include <chrono>
    #include <cstring>
    #include <variant>
    #include <cmath>
    #include <unordered_map>
    #include <utility>
    
    #define NOMINMAX
    #include <algorithm>

#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include <libdas/include/Api.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/TextureReader.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <BufferAlignment.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Missing.h>
    #include <Renderer.h>
    #include <GPUMemoryManager.h>
#endif

#include <VulkanHelpers.h>
#include <VulkanInstanceCreator.h>
#include <VulkanSwapchainCreator.h>
#include <VulkanPipelineCreator.h>
#include <VulkanDescriptorSetLayoutCreator.h>
#include <VulkanDescriptorAllocator.h>



namespace DENG {

    class DENG_API VulkanRenderer : public Renderer {
        private:
            Vulkan::InstanceCreator *mp_instance_creator;
            Vulkan::SwapchainCreator *mp_swapchain_creator;
            std::vector<Vulkan::PipelineCreator> m_pipeline_creators;
            std::vector<Vulkan::DescriptorSetLayoutCreator> m_descriptor_set_layout_creators;

            // per shader descriptor pools / sets creators
            std::vector<Vulkan::DescriptorAllocator> m_shader_desc_allocators;
            std::vector<uint32_t> m_shader_descriptor_set_index_table;

            // per mesh descriptor pools / sets creators
            std::vector<Vulkan::DescriptorAllocator> m_mesh_desc_allocators;
            std::vector<uint32_t> m_mesh_descriptor_set_index_table;
            
            // locally managed vulkan resources
            VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_2_BIT; // tmp
            VkDeviceSize m_uniform_size = DEFAULT_UNIFORM_SIZE;
            VkDeviceSize m_buffer_size = DEFAULT_BUFFER_SIZE;

            //  Main memory is usually splitted into vertex regions and index regions, but not always
            //  [ [ VERTICES ] [ INDICES ] ]
            VkBuffer m_main_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_main_memory = VK_NULL_HANDLE;

            // uniform buffer
            VkBuffer m_uniform_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_uniform_memory = VK_NULL_HANDLE;

            VkCommandPool m_command_pool = VK_NULL_HANDLE;
            std::vector<VkSemaphore> m_render_finished_semaphores;
            std::vector<VkSemaphore> m_image_available_semaphores;
            std::vector<VkFence> m_flight_fences;
            std::vector<VkCommandBuffer> m_cmd_buffers;

            // color and depth image resources
            VkImage m_color_image = VK_NULL_HANDLE;
            VkDeviceMemory m_color_image_memory = VK_NULL_HANDLE;
            VkImageView m_color_image_view = VK_NULL_HANDLE;

            VkImage m_depth_image = VK_NULL_HANDLE;
            VkDeviceMemory m_depth_image_memory = VK_NULL_HANDLE;
            VkImageView m_depth_image_view = VK_NULL_HANDLE;

            // framebuffers and command buffers
            std::vector<VkFramebuffer> m_framebuffers;
            std::vector<VkCommandBuffer> m_command_buffers;
            uint32_t m_current_frame = 0;
            bool m_is_init = false;

            std::unordered_map<std::string, Vulkan::TextureData> m_vulkan_texture_handles;

        private:
            void _CreateCommandPool();
            void _CreateSemaphores();
            void _AllocateBufferResources();
            void _AllocateUniformBuffer();
            void _ReallocateBufferResources(VkDeviceSize _old_size);
            void _ReallocateUniformBuffer();
            void _CreateColorResources();
            void _CreateDepthResources();
            void _CreateFrameBuffers();
            void _AllocateCommandBuffers();
            void _RecordCommandBuffer(uint32_t _imgi);
            void _CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels);
            void _CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels);
            void _Resize();


        public:
            VulkanRenderer(Window &_win, const RendererConfig &_conf);
            ~VulkanRenderer();

            virtual void PushTextureFromFile(const std::string &_name, const std::string &_file_name) override;
            virtual void PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) override;
            virtual void RemoveTexture(const std::string &_name) override;

            virtual std::vector<std::string> GetTextureNames() override;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) override;
            virtual void LoadShaders(uint32_t _ubo_size = 0) override;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) override;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;

    };
}

#endif
