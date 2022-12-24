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
    #include <queue>
    #include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/TextureDatabase.h"
    #include "deng/Renderer.h"
    #include "deng/VulkanHelpers.h"
    #include "deng/VulkanInstanceCreator.h"
    #include "deng/VulkanSwapchainCreator.h"
    #include "deng/VulkanPipelineCreator.h"
    #include "deng/VulkanDescriptorSetLayoutCreator.h"
    #include "deng/VulkanDescriptorAllocator.h"
    #include "deng/VulkanFramebuffer.h"
#endif

namespace DENG {
    namespace Vulkan {

        class Framebuffer {
            private:
                InstanceCreator &m_instance_creator;
                SwapchainCreator* m_swapchain_creator = nullptr;
                VkBuffer &m_uniform;
                VkBuffer &m_main_buffer;
                VkSampleCountFlagBits m_sample_count;
                
                std::vector<Vulkan::PipelineCreator> m_pipeline_creators;
                std::vector<uint32_t> m_pipeline_index_table;
                std::vector<Vulkan::DescriptorSetLayoutCreator> m_shader_descriptor_set_layout_creators;
                std::vector<Vulkan::DescriptorSetLayoutCreator> m_mesh_descriptor_set_layout_creators;

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
                uint32_t m_color_resolve_image_id = UINT32_MAX;
                uint32_t m_depth_image_id = UINT32_MAX;
                std::vector<uint32_t> m_framebuffer_image_ids;

                // missing resources
                const uint32_t m_missing_2d;
                const uint32_t m_missing_3d;

                VkRenderPass m_renderpass;
                bool m_no_swapchain;
                bool m_command_buffer_recording_bit = false;

                TRS::Point2D<uint32_t> m_extent;

                uint32_t m_current_frame;

            private:
                void _CreateColorResources();
                void _CreateDepthResources();
                void _CreateFramebuffers();
                void _CreateFramebufferImage();
                void _CleanPipelineResources();
                void _CreateCommandPool();
                void _AllocateCommandBuffers();
                void _CreateSynchronisationPrimitives();

                void _CheckAndCreateShaderDescriptors(const MeshReference &_ref, const std::vector<ShaderModule*>& _modules);
                void _CheckAndCreateMeshDescriptors(const MeshReference& _ref, uint32_t _mesh_id);

            public:
                Framebuffer(
                    InstanceCreator& _ic,
                    VkBuffer &_uniform, 
                    VkBuffer &_main, 
                    VkSampleCountFlagBits _sample_c,
                    TRS::Point2D<uint32_t> _extent,
                    uint32_t _missing_2d,
                    uint32_t _missing_3d,
                    bool _no_swapchain = false
                );
                Framebuffer(Framebuffer &&_fb) noexcept = default;
                ~Framebuffer();

                void LoadData();
                void ReloadResources();
                void RecreateDescriptorSets();
                void RecreateFramebuffer();
                void DestroyFramebuffer(bool _remove_from_registry = true);

                inline void ClearFrame() {
                    vkWaitForFences(m_instance_creator.GetDevice(), 1, &m_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
                }
                void StartCommandBufferRecording(TRS::Vector4<float> _clear_color);
                void Draw(const MeshReference &_ref, uint32_t _mesh_id, const std::vector<ShaderModule*> &_modules);
                void EndCommandBufferRecording();
                void Render();

                // only call this function if using swapchain images ! ! !
                VkResult Present();

                inline bool GetCommandBufferRecordingBit() {
                    return m_command_buffer_recording_bit;
                }

                inline bool GetSwapchainBit() {
                    return !m_no_swapchain;
                }

                inline SwapchainCreator* GetSwapchainCreator() {
                    DENG_ASSERT(m_swapchain_creator);
                    return m_swapchain_creator;
                }

                inline VkCommandPool GetCommandPool() {
                    return m_command_pool;
                }

                inline VkRenderPass GetRenderPass() {
                    return m_renderpass;
                }

                inline const std::vector<uint32_t> &GetFramebufferImageIds() const {
                    return m_framebuffer_image_ids;
                }

                inline PipelineCreator& GetPipelineCreator(uint32_t _id) {
                    DENG_ASSERT(_id < static_cast<uint32_t>(m_pipeline_creators.size()));
                    return m_pipeline_creators[_id];
                }

                inline VkSemaphore GetSwapchainImageAcquisitionSemaphore() {
                    return m_image_available_semaphores[m_current_frame];
                }

                inline VkSemaphore GetRenderFinishedSemaphore() {
                    return m_render_finished_semaphores[m_current_frame];
                }

                inline void SetExtent(TRS::Point2D<uint32_t> _ext) {
                    m_extent = _ext;
                }
        };
    }
}

#endif
