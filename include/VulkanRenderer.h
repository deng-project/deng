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
    #include <RenderState.h>
    #include <GPUMemoryManager.h>
#endif

#include <VulkanHelpers.h>
#include <VulkanInstanceCreator.h>
#include <VulkanSwapchainCreator.h>
#include <VulkanPipelineCreator.h>
#include <VulkanDescriptorSetLayoutCreator.h>
#include <VulkanDescriptorAllocator.h>
#include <VulkanFramebuffer.h>


namespace DENG {

    namespace Vulkan {
        void Initialise();
    }

    class DENG_API VulkanRenderer : public Renderer {
        private:
            const VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_1_BIT;
            Vulkan::InstanceCreator m_instance_creator;
            Vulkan::SwapchainCreator m_swapchain_creator;

            // texture handles
            std::unordered_map<std::string, Vulkan::TextureData> m_vulkan_texture_handles;

            // framebuffers
            std::unordered_map<std::string, Vulkan::Framebuffer> m_framebuffers;
            
            // locally managed vulkan resources
            VkDeviceSize m_uniform_size = DEFAULT_UNIFORM_SIZE;
            VkDeviceSize m_buffer_size = DEFAULT_BUFFER_SIZE;

            //  Main memory is usually splitted into vertex regions and index regions, but not always
            //  [ [ VERTICES ] [ INDICES ] ]
            VkBuffer m_main_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_main_memory = VK_NULL_HANDLE;

            // uniform buffer
            VkBuffer m_uniform_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_uniform_memory = VK_NULL_HANDLE;
            uint32_t m_current_frame = 0;
            bool m_is_init = false;

        private:
            void _AllocateBufferResources();
            void _AllocateUniformBuffer();
            void _ReallocateBufferResources(VkDeviceSize _old_size);
            void _ReallocateUniformBuffer();
            void _CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels);
            void _CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels);
            void _Resize();


        public:
            VulkanRenderer(Window &_win, const RendererConfig &_conf);
            ~VulkanRenderer();

            virtual void PushFramebuffer(const FramebufferDrawData &_fb) override;
            virtual void PushTextureFromFile(const std::string &_name, const std::string &_file_name) override;
            virtual void PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) override;
            virtual void RemoveTexture(const std::string &_name) override;

            virtual std::vector<std::string> GetTextureNames() override;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) override;
            virtual void LoadShaders() override;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) override;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;

    };
}

#endif
