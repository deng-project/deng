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
    #include <chrono>
    #include <cstring>
    #include <variant>
    #include <cmath>
    #include <queue>
    #include <list>
    #include <utility>
    #include <memory>
    #include <thread>
    
    #define NOMINMAX
    #include <algorithm>

#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/BufferAlignment.h"
    #include "deng/Window.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Missing.h"
    #include "deng/Renderer.h"
    #include "deng/RenderState.h"
    #include "deng/GPUMemoryManager.h"
    #include "deng/TextureDatabase.h"

    #define CALC_MIPLVL(_x, _y) (static_cast<uint32_t>(std::floor(std::log2(std::max(static_cast<double>(_x), static_cast<double>(_y))))))
#endif

#include "deng/VulkanHelpers.h"
#include "deng/VulkanInstanceCreator.h"
#include "deng/VulkanSwapchainCreator.h"
#include "deng/VulkanPipelineCreator.h"
#include "deng/VulkanDescriptorSetLayoutCreator.h"
#include "deng/VulkanDescriptorAllocator.h"
#include "deng/VulkanFramebuffer.h"


namespace DENG {

    namespace Vulkan {
        void DENG_API Initialise();
    }

    class DENG_API VulkanRenderer : public Renderer {
        private:
            TRS::Point2D<uint32_t> m_previous_canvas;
            const VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_1_BIT;
            Vulkan::InstanceCreator m_instance_creator;

            // framebuffers
            std::vector<Vulkan::Framebuffer*> m_framebuffers;
            
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

            uint32_t m_missing_2d = 0;
            uint32_t m_missing_3d = 0;
            bool m_is_init = false;

        private:
            void _AllocateBufferResources();
            void _AllocateUniformBuffer();
            void _ReallocateBufferResources(VkDeviceSize _old_size);
            void _ReallocateUniformBuffer();
            void _CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels, uint32_t _array_count);
            void _CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels);
            void _Resize();

            void _CreateVendorTextureImages(uint32_t _id);
            
            void _CheckAndCopyTextures();
            void _CheckAndDeleteTextures();
            void _CheckAndDeleteMeshes();
            void _CheckAndRemoveShaders();


        public:
            VulkanRenderer(const RendererConfig &_conf);
            ~VulkanRenderer();

            virtual FramebufferIndices AddFramebuffer(TRS::Point2D<uint32_t> _extent) override;
            virtual std::vector<std::string> GetTextureNames() override;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) override;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) override;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;

    };
}

#endif
