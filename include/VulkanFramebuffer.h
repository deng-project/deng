// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanFramebuffer.h - Vulkan framebuffer management class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_FRAMEBUFFER_H
#define VULKAN_FRAMEBUFFER_H

#ifdef VULKAN_FRAMEBUFFER_CPP
    #include <vector>
    #include <string>
    #include <array>
    #include <variant>
    #include <unordered_map>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <VulkanHelpers.h>
    #include <VulkanPipelineCreator.h>
    #include <VulkanDescriptorSetLayoutCreator.h>
    #include <VulkanDescriptorAllocator.h>
    #include <VulkanInstanceCreator.h>
    #include <VulkanSwapchainCreator.h>
#endif

namespace DENG {
    namespace Vulkan {

        class Framebuffer {
            private:
                const InstanceCreator &m_instance_creator;
                VkBuffer &m_uniform;
                VkBuffer &m_main_buffer;
                VkFormat m_format;
                VkSampleCountFlagBits m_sample_count;
                std::string m_framebuffer_name;
                const std::unordered_map<std::string, FramebufferDrawData> &m_framebuffer_draws;
                std::vector<Vulkan::TextureData> m_framebuffer_images;
                const std::unordered_map<std::string, Vulkan::TextureData> &m_misc_textures;

                std::vector<Vulkan::PipelineCreator> m_pipeline_creators;
                std::vector<Vulkan::DescriptorSetLayoutCreator> m_descriptor_set_layout_creators;

                // per shader descriptor pools / sets creators
                std::vector<Vulkan::DescriptorAllocator> m_shader_desc_allocators;
                std::vector<uint32_t> m_shader_descriptor_set_index_table;

                // per mesh descriptor pools / sets creators
                std::vector<Vulkan::DescriptorAllocator> m_mesh_desc_allocators;
                std::vector<uint32_t> m_mesh_descriptor_set_index_table;

                VkCommandPool m_command_pool;
                std::vector<VkCommandBuffer> m_command_buffers;
                std::vector<VkSemaphore> m_render_finished_semaphores;
                std::vector<VkSemaphore> m_image_available_semaphores;
                std::vector<VkFence> m_flight_fences;
                std::vector<VkFramebuffer> m_framebuffers;

                // color resolve and depth image resources
                VkImage m_color_resolve_image;
                VkImageView m_color_resolve_image_view;
                VkDeviceMemory m_color_resolve_image_memory;

                VkImage m_depth_image;
                VkImageView m_depth_image_view;
                VkDeviceMemory m_depth_image_memory;

                VkRenderPass m_renderpass;
                bool m_no_swapchain;

                uint32_t m_current_frame;

            private:
                void _CreateColorResources();
                void _CreateDepthResources();
                void _CreateFramebuffers();
                void _CleanPipelineResources();
                void _CreateCommandPool();
                void _AllocateCommandBuffers();
                void _CreateSynchronisationPrimitives();
                void _RecordCommandBuffers(const Libdas::Vector4<float> _clear_color, uint32_t _imgi);

            public:
                Framebuffer(
                    const InstanceCreator &_ic, 
                    VkBuffer &_uniform, 
                    VkBuffer &_main, 
                    VkFormat _format, 
                    VkSampleCountFlagBits _sample_c, 
                    const std::string &_fb_name,
                    const std::unordered_map<std::string, FramebufferDrawData> &_fb_draws,
                    const std::vector<Vulkan::TextureData> &_fb_images,
                    const std::unordered_map<std::string, Vulkan::TextureData> &_misc_images, 
                    bool _no_swapchain = false
                );
                Framebuffer(Framebuffer &&_fb) noexcept = default;
                ~Framebuffer();

                void LoadData();
                void ReloadResources();
                void RecreateDescriptorSets();
                void RecreateFramebuffer();
                void DestroyFramebuffer();
                inline void ClearFrame() {
                    vkWaitForFences(m_instance_creator.GetDevice(), 1, &m_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
                }
                void Render(const Libdas::Vector4<float> _clear_color, uint32_t _imgi = 0);

                // only call this function if using swapchain images ! ! !
                VkResult Present(VkSwapchainKHR &_swapchain, uint32_t _imgi);

                inline void NewFramebufferImages(const std::vector<Vulkan::TextureData> &_images) {
                    m_framebuffer_images = _images;
                }

                inline VkCommandPool GetCommandPool() {
                    return m_command_pool;
                }

                inline const std::vector<Vulkan::TextureData> &GetFramebufferImages() {
                    return m_framebuffer_images;
                }

                inline VkSemaphore GetSwapchainImageAcquisitionSemaphore() {
                    return m_image_available_semaphores[m_current_frame];
                }

                inline VkSemaphore GetRenderFinishedSemaphore() {
                    return m_render_finished_semaphores[m_current_frame];
                }
        };
    }
}

#endif
