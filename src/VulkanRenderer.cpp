// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.cpp - Vulkan renderer class implementation
// author: Karl-Mihkel Ott

#define VULKAN_RENDERER_CPP
#include "deng/VulkanRenderer.h"


namespace DENG {

    namespace Vulkan {
        void Initialise() {
            RenderState *rs = RenderState::GetInstance();
            if(rs->GetPrimary() == RENDERER_TYPE_UNKNOWN)
                rs->SetPrimary(RENDERER_TYPE_VULKAN);
        }
    }


    VulkanRenderer::VulkanRenderer(const RendererConfig &_conf) : 
        Renderer(_conf),
        m_instance_creator(m_conf),
        m_swapchain_creator(m_instance_creator, m_conf.canvas_size, m_sample_count, m_conf)
    {
        m_previous_canvas = m_conf.canvas_size;
        RenderState *rs = RenderState::GetInstance();
        m_id = rs->RegisterRenderer(RENDERER_TYPE_VULKAN, time(NULL));

        // create main swapchain framebuffer
        VkExtent2D ext = m_instance_creator.GetSurfaceCapabilities().currentExtent;
        m_framebuffer_draws.insert({
            MAIN_FRAMEBUFFER_NAME,
            {
                MAIN_FRAMEBUFFER_NAME,
                {}, {},
                {
                    ext.width,
                    ext.height
                }
            }
        });

        m_framebuffers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(MAIN_FRAMEBUFFER_NAME),
            std::forward_as_tuple(std::make_shared<Vulkan::Framebuffer>(
                m_instance_creator,
                m_uniform_buffer,
                m_main_buffer,
                m_swapchain_creator.GetSwapchainFormat(),
                m_sample_count,
                MAIN_FRAMEBUFFER_NAME,
                m_framebuffer_draws,
                m_swapchain_creator.GetSwapchainImages(),
                m_vulkan_texture_handles
            ))
        );
        _AllocateBufferResources();

        // push missing texture 
        int x, y, size;
        char *data = GetMissingTextureRGBA(x, y, size);
        PushTextureFromMemory(MISSING_TEXTURE_NAME, data, x, y, 4);

        // push a missing cubemap
        std::array<Libdas::TextureReader, 6> readers = {
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false)),
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false)),
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false)),
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false)),
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false)),
            std::move(Libdas::TextureReader(std::make_pair(data, size), 64, 64, false))
        };

        PushCubemapFromMemory(MISSING_3D_TEXTURE_NAME, readers);
    }


    VulkanRenderer::~VulkanRenderer() {
        RenderState *rs = RenderState::GetInstance();
        rs->RemoveRenderer(m_id);
        vkDeviceWaitIdle(m_instance_creator.GetDevice());

        // destroy all secondary framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end();) {
            it = m_framebuffers.erase(it);
        }

        // destroy texture resources
        for(auto it = m_vulkan_texture_handles.begin(); it != m_vulkan_texture_handles.end(); it++) {
            vkDestroySampler(m_instance_creator.GetDevice(), it->second.sampler, nullptr);
            vkDestroyImageView(m_instance_creator.GetDevice(), it->second.image_view, nullptr);
            vkDestroyImage(m_instance_creator.GetDevice(), it->second.image, nullptr);
            vkFreeMemory(m_instance_creator.GetDevice(), it->second.memory, nullptr);
        }

        // free main buffers
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_main_buffer, NULL);
        vkFreeMemory(m_instance_creator.GetDevice(), m_main_memory, NULL);

        // free uniform buffers
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_uniform_buffer, nullptr);
        vkFreeMemory(m_instance_creator.GetDevice(), m_uniform_memory, nullptr);
    }


    void VulkanRenderer::PushFramebuffer(const FramebufferDrawData &_fb) {
        // check if the framebuffer already exists
        DENG_ASSERT(m_framebuffer_draws.find(_fb.image_name) == m_framebuffer_draws.end());
        DENG_ASSERT(m_framebuffers.find(_fb.image_name) == m_framebuffers.end());
        DENG_ASSERT(m_vulkan_texture_handles.find(_fb.image_name) == m_vulkan_texture_handles.end());

        m_framebuffer_draws[_fb.image_name] = _fb;

        // create a new framebuffer image
        Vulkan::TextureData texture = {};
        VkMemoryRequirements mem_req = Vulkan::_CreateImage(
            m_instance_creator.GetDevice(), 
            texture.image, 
            _fb.extent.x, 
            _fb.extent.y, 
            1,
            1,
            VK_FORMAT_R8G8B8A8_UNORM, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT,
            0);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, texture.memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(m_instance_creator.GetDevice(), texture.image, texture.memory, 0);

        // create image view
        VkImageViewCreateInfo image_view_info = Vulkan::_GetImageViewInfo(texture.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
        if(vkCreateImageView(m_instance_creator.GetDevice(), &image_view_info, nullptr, &texture.image_view) != VK_SUCCESS)
            VK_RES_ERR("Failed to create a framebuffer image for framebuffer draw " + _fb.image_name);

        // create sampler
        _CreateTextureSampler(texture, 1);

        // create a framebuffer
        std::vector<Vulkan::TextureData> images = { texture };
        m_framebuffers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(_fb.image_name), 
            std::forward_as_tuple(std::make_shared<Vulkan::Framebuffer>(
                m_instance_creator, 
                m_uniform_buffer, 
                m_main_buffer, 
                VK_FORMAT_R8G8B8A8_UNORM, 
                m_sample_count,
                _fb.image_name,
                m_framebuffer_draws,
                images,
                m_vulkan_texture_handles, 
                true
            ))
        );

        m_vulkan_texture_handles[_fb.image_name] = texture;
    }


    void VulkanRenderer::PushTextureFromFile(const std::string &_name, const std::string &_file_name) {
        Libdas::TextureReader reader(_file_name, true);
        int x, y;
        size_t len;
        const char *raw = reader.GetRawBuffer(x, y, len);

        PushTextureFromMemory(_name, raw, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4);
    }
    

    void VulkanRenderer::PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // check if texture with specified key already exists
        if(m_vulkan_texture_handles.find(_name) != m_vulkan_texture_handles.end()) {
            throw std::runtime_error("Texture with name " + _name + " already exists");
            return;
        }
        
        const VkDeviceSize size = static_cast<VkDeviceSize>(_width * _height * _bit_depth);
        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();

        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(m_instance_creator.GetDevice(), staging_buffer, staging_memory, 0);

        Vulkan::_CopyToBufferMemory(m_instance_creator.GetDevice(), size, _raw_data, staging_memory, 0);
        
        // create texture image instances
        Vulkan::TextureData texture_data;
        texture_data.width = _width;
        texture_data.height = _height;
        texture_data.depth = _bit_depth;

        uint32_t mip_levels = CALC_MIPLVL(_width, _height);
        mem_req = Vulkan::_CreateImage(
            m_instance_creator.GetDevice(), 
            texture_data.image, 
            _width, 
            _height,
            mip_levels,
            1,
            VK_FORMAT_R8G8B8A8_UNORM, 
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_SAMPLE_COUNT_1_BIT,
            0);

        Vulkan::_AllocateMemory(
            m_instance_creator.GetDevice(), 
            m_instance_creator.GetPhysicalDevice(), 
            mem_req.size, 
            texture_data.memory, 
            mem_req.memoryTypeBits, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindImageMemory(m_instance_creator.GetDevice(), texture_data.image, texture_data.memory, 0);
        Vulkan::_TransitionImageLayout(
            m_instance_creator.GetDevice(), 
            texture_data.image, 
            pool, 
            m_instance_creator.GetGraphicsQueue(),
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            mip_levels,
            1);

        Vulkan::_CopyBufferToImage(
            m_instance_creator.GetDevice(), 
            pool, 
            m_instance_creator.GetGraphicsQueue(), 
            staging_buffer, 
            texture_data.image, 
            _width, 
            _height,
            1);

        vkDestroyBuffer(m_instance_creator.GetDevice(), staging_buffer, nullptr);
        vkFreeMemory(m_instance_creator.GetDevice(), staging_memory, nullptr);
        _CreateMipmaps(texture_data.image, _width, _height, mip_levels, 1);

        // create texture image view
        VkImageViewCreateInfo image_view_info = {};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = texture_data.image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = mip_levels;
        if(vkCreateImageView(m_instance_creator.GetDevice(), &image_view_info, nullptr, &texture_data.image_view) != VK_SUCCESS)
            VK_RES_ERR("Failed to create texture image view");

        _CreateTextureSampler(texture_data, mip_levels);

        m_vulkan_texture_handles[_name] = texture_data;
    }


    void VulkanRenderer::PushCubemapFromMemory(
        const std::string& _name,
        std::array<Libdas::TextureReader, 6> &_readers
    ) {
        // texture already exists
        if (m_vulkan_texture_handles.find(_name) != m_vulkan_texture_handles.end()) {
            throw std::runtime_error("Texture with name " + _name + " already exists");
            return;
        }

        // count the total buffer size
        VkDeviceSize size = 0;
        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();

        // tmp until Libdas::TextureReader is fixed
        int base_x = 0, base_y = 0;
        for (auto it = _readers.begin(); it != _readers.end(); it++) {
            int x, y;
            size_t len;
            it->GetRawBuffer(x, y, len);

            if (base_x && base_y && (x != base_x || y != base_y)) {
                throw std::runtime_error("Invalid cubemap image dimentions " + std::to_string(x) + "x" + std::to_string(y));
                return;
            } else if (!base_x && !base_y) {
                base_x = x;
                base_y = y;
            }
            size += static_cast<VkDeviceSize>(len);
        }

        // allocate the staging buffer
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, staging_memory,
                                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(m_instance_creator.GetDevice(), staging_buffer, staging_memory, 0);

        VkDeviceSize offset = 0;
        for (auto it = _readers.begin(); it != _readers.end(); it++) {
            int x, y;
            size_t len;
            const char* data = it->GetRawBuffer(x, y, len);
            Vulkan::_CopyToBufferMemory(m_instance_creator.GetDevice(), len, data, staging_memory, offset);
            offset += len;
        }
        
        Vulkan::TextureData texture;
        texture.width = base_x;
        texture.height = base_y;
        texture.depth = 4;

        mem_req = Vulkan::_CreateImage(
            m_instance_creator.GetDevice(),
            texture.image,
            base_x,
            base_y,
            1,
            6,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);

        Vulkan::_AllocateMemory(
            m_instance_creator.GetDevice(), 
            m_instance_creator.GetPhysicalDevice(), 
            mem_req.size, 
            texture.memory,
            mem_req.memoryTypeBits, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkBindImageMemory(m_instance_creator.GetDevice(), texture.image, texture.memory, 0);
        Vulkan::_TransitionImageLayout(
            m_instance_creator.GetDevice(),
            texture.image,
            pool,
            m_instance_creator.GetGraphicsQueue(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            6);

        Vulkan::_CopyBufferToImage(
            m_instance_creator.GetDevice(),
            pool,
            m_instance_creator.GetGraphicsQueue(),
            staging_buffer,
            texture.image,
            static_cast<uint32_t>(base_x),
            static_cast<uint32_t>(base_y),
            6);

        Vulkan::_TransitionImageLayout(
            m_instance_creator.GetDevice(),
            texture.image,
            pool,
            m_instance_creator.GetGraphicsQueue(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            1,
            6);

        // destroy staging buffers
        vkDestroyBuffer(m_instance_creator.GetDevice(), staging_buffer, nullptr);
        vkFreeMemory(m_instance_creator.GetDevice(), staging_memory, nullptr);
        
        VkImageViewCreateInfo image_view_info = {};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.image = texture.image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        image_view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        image_view_info.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };

        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 6;
        vkCreateImageView(m_instance_creator.GetDevice(), &image_view_info, nullptr, &texture.image_view);
        _CreateTextureSampler(texture, 1);

        m_vulkan_texture_handles[_name] = texture;
    }


    void VulkanRenderer::GetTexturePointer(const std::string& _name, RawTextureData &_out_data) {
        auto texture = m_vulkan_texture_handles.find(_name);
        _out_data.width = texture->second.width;
        _out_data.height = texture->second.height;
        _out_data.depth = texture->second.depth;

        // allocate a temporary staging buffer
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;

        const VkDeviceSize size = static_cast<VkDeviceSize>(_out_data.width * _out_data.height * _out_data.depth);
        
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, staging_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(m_instance_creator.GetDevice(), staging_buffer, staging_memory, 0);


        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();
        VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
        Vulkan::_BeginCommandBufferSingleCommand(m_instance_creator.GetDevice(), pool, cmd_buf);
            VkBufferImageCopy copy = {};
            copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy.imageSubresource.layerCount = 1;
            copy.imageExtent = VkExtent3D{ texture->second.width, texture->second.height, 1 };
            vkCmdCopyImageToBuffer(cmd_buf, texture->second.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, staging_buffer, 1, &copy);
        Vulkan::_EndCommandBufferSingleCommand(m_instance_creator.GetDevice(), m_instance_creator.GetGraphicsQueue(), pool, cmd_buf);
    
        // now copy from staging buffer to local memory
        void* tmp = nullptr;
        _out_data.raw = new unsigned char[size];
        vkMapMemory(m_instance_creator.GetDevice(), staging_memory, 0, size, 0, &tmp);
        memcpy(_out_data.raw, tmp, size);
        vkUnmapMemory(m_instance_creator.GetDevice(), staging_memory);
    
        vkFreeMemory(m_instance_creator.GetDevice(), staging_memory, nullptr);
        vkDestroyBuffer(m_instance_creator.GetDevice(), staging_buffer, nullptr);
    }


    void VulkanRenderer::RemoveTexture(const std::string &_name) {
        if(m_vulkan_texture_handles.find(_name) == m_vulkan_texture_handles.end()) {
            std::cerr << "Texture with name " << _name << " does not exist" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        Vulkan::TextureData &tex = m_vulkan_texture_handles[_name];

        vkDestroySampler(m_instance_creator.GetDevice(), tex.sampler, nullptr);
        vkDestroyImageView(m_instance_creator.GetDevice(), tex.image_view, nullptr);
        vkDestroyImage(m_instance_creator.GetDevice(), tex.image, nullptr);
        vkFreeMemory(m_instance_creator.GetDevice(), tex.memory, nullptr);

        m_vulkan_texture_handles.erase(_name);
    }


    std::vector<std::string> VulkanRenderer::GetTextureNames() {
        std::vector<std::string> names;
        names.reserve(m_vulkan_texture_handles.size());

        for(auto it = m_vulkan_texture_handles.begin(); it != m_vulkan_texture_handles.end(); it++)
            names.push_back(it->first);

        return names;
    }


    uint32_t VulkanRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, m_instance_creator.GetMinimalUniformBufferAlignment());
    }


    void VulkanRenderer::LoadShaders() {
        if (!m_is_init) {
            _AllocateUniformBuffer();
        }

        // load framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++)
            it->second->LoadData();

        m_is_init = true;
    }


    void VulkanRenderer::ReloadShaderModule(uint32_t _shader_id, const std::string& _framebuffer) {
        DENG_ASSERT(m_framebuffers.find(_framebuffer) != m_framebuffers.end());
        auto& fb = m_framebuffers.find(_framebuffer)->second;
        auto& fb_draw = m_framebuffer_draws.find(_framebuffer)->second;
        auto& pipeline = fb->GetPipelineCreator(_shader_id);

        VkExtent2D ext = { fb_draw.extent.x, fb_draw.extent.y };
        vkDeviceWaitIdle(m_instance_creator.GetDevice());
        pipeline.DestroyPipelineData();
        pipeline.SetShaderModule(GetShaderModule(_shader_id, _framebuffer));
        pipeline.RecreatePipeline(fb->GetRenderPass(), ext, true);
    }


    void VulkanRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_size + _offset) >= m_uniform_size) {
            m_uniform_size = (_size + _offset) * 3 / 2;
            _ReallocateUniformBuffer();

            for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
                it->second->RecreateDescriptorSets();
            }
        }

        if(_raw_data) {
            Vulkan::_CopyToBufferMemory(m_instance_creator.GetDevice(), static_cast<VkDeviceSize>(_size), _raw_data, m_uniform_memory, static_cast<VkDeviceSize>(_offset));
        }
    }


    void VulkanRenderer::UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_buffer_size) {
            VkDeviceSize old_size = m_buffer_size;
            m_buffer_size = (_raw_data.second + _offset) * 3 / 2;
            _ReallocateBufferResources(old_size);
        }

        Vulkan::_ImplicitDataToBufferCopy(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), pool, m_instance_creator.GetGraphicsQueue(), static_cast<VkDeviceSize>(_raw_data.second), 
                                          reinterpret_cast<const void*>(_raw_data.first), m_main_buffer, static_cast<VkDeviceSize>(_offset));
    }


    void VulkanRenderer::ClearFrame() {
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            if(it->first == MAIN_FRAMEBUFFER_NAME)
                continue;

            it->second->ClearFrame();
        }

        m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->ClearFrame();
    }


    void VulkanRenderer::RenderFrame() {
        // acquire next images to draw
        VkSemaphore swapchain_image_semaphore = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetSwapchainImageAcquisitionSemaphore();
        uint32_t imgi;
        VkResult res = vkAcquireNextImageKHR(m_instance_creator.GetDevice(), m_swapchain_creator.GetSwapchain(), UINT64_MAX, swapchain_image_semaphore, VK_NULL_HANDLE, &imgi);

        if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            _Resize();
            return;
        } else if(res != VK_SUCCESS) {
            VK_FRAME_ERR("Failed to acquire next swapchain image");
        }

        // render misc framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            if(it->first == MAIN_FRAMEBUFFER_NAME)
                continue;

            it->second->Render(m_conf.clear_color);
        }

        Vulkan::Framebuffer &fb = *m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second;

        fb.Render(m_conf.clear_color, imgi);
        res = fb.Present(m_swapchain_creator.GetSwapchain(), imgi);

        if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
            _Resize();
        else if(res != VK_SUCCESS)
            VK_FRAME_ERR("Failed to present swapchain image");
    }


    void VulkanRenderer::_AllocateBufferResources() {
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), m_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_main_buffer);

        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, m_main_memory, 
                                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindBufferMemory(m_instance_creator.GetDevice(), m_main_buffer, m_main_memory, 0);
    }


    void VulkanRenderer::_AllocateUniformBuffer() {
        // find the maximum offset and size for uniform block
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        const uint32_t max_size = mem_man->GetMaxUniformOffset();

        m_uniform_size = DEFAULT_UNIFORM_SIZE > max_size ? DEFAULT_UNIFORM_SIZE : max_size;

        // create and allocate uniform buffer resourses
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), m_uniform_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_uniform_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, m_uniform_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        m_uniform_size = mem_req.size;
        vkBindBufferMemory(m_instance_creator.GetDevice(), m_uniform_buffer, m_uniform_memory, 0);
    }


    void VulkanRenderer::_ReallocateBufferResources(VkDeviceSize _old_size) {
        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();
        // step 1: create staging buffer
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), _old_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, staging_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(m_instance_creator.GetDevice(), staging_buffer, staging_memory, 0);

        Vulkan::_CopyBufferToBuffer(m_instance_creator.GetDevice(), pool, m_instance_creator.GetGraphicsQueue(), m_main_buffer, staging_buffer, _old_size, 0, 0);

        // step 2: free to old buffer instance and allocate new resources
        // NOTE: new supported sizes must be calculated beforehand
        vkFreeMemory(m_instance_creator.GetDevice(), m_main_memory, NULL);
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_main_buffer, NULL);
        _AllocateBufferResources();

        // step 3: copy data to new offsets in new buffer
        Vulkan::_CopyBufferToBuffer(m_instance_creator.GetDevice(), pool, m_instance_creator.GetGraphicsQueue(), staging_buffer, m_main_buffer, _old_size, 0, 0);
    }


    void VulkanRenderer::_ReallocateUniformBuffer() {
        // step 1: free all previous resources
        vkFreeMemory(m_instance_creator.GetDevice(), m_uniform_memory, nullptr);
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_uniform_buffer, nullptr);

        // step 2: allocate new uniform buffer
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(m_instance_creator.GetDevice(), m_uniform_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_uniform_buffer);
        Vulkan::_AllocateMemory(m_instance_creator.GetDevice(), m_instance_creator.GetPhysicalDevice(), mem_req.size, m_uniform_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        m_uniform_size = mem_req.size;
        vkBindBufferMemory(m_instance_creator.GetDevice(), m_uniform_buffer, m_uniform_memory, 0);
    }


    void VulkanRenderer::_CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels, uint32_t _array_count) {
        const VkCommandPool pool = m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second->GetCommandPool();
        // OPTIONAL:
        // check if image format supports linear blitting
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(m_instance_creator.GetPhysicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &format_props);
        DENG_ASSERT(format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);

        VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
        Vulkan::_BeginCommandBufferSingleCommand(m_instance_creator.GetDevice(), pool, cmd_buf);

        VkImageMemoryBarrier mem_barrier = {};
        mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        mem_barrier.image = _img;
        mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        mem_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        mem_barrier.subresourceRange.baseArrayLayer = 0;
        mem_barrier.subresourceRange.layerCount = _array_count;
        mem_barrier.subresourceRange.baseMipLevel = 0;
        mem_barrier.subresourceRange.levelCount = 1;

        for(uint32_t i = 1; i < _mip_levels; i++) {
            // set pipeline ready for transfer processing
            mem_barrier.subresourceRange.baseMipLevel = i - 1;
            mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            mem_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            mem_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);
            
            // set up image blit structure
            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { static_cast<int32_t>(_width), static_cast<int32_t>(_height), 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.layerCount = _array_count;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { 
                static_cast<int32_t>(_width) > 1 ? static_cast<int32_t>(_width) / 2 : 1, 
                static_cast<int32_t>(_height) > 1 ? static_cast<int32_t>(_height) / 2 : 1,
                1
            };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.layerCount = _array_count;

            vkCmdBlitImage(cmd_buf, _img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            // set pipeline ready for final transfer into fragment shader
            mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

            if(_width > 1) _width /= 2;
            if(_height > 1) _height /= 2;
        }

        // final mip level transitioning
        mem_barrier.subresourceRange.baseMipLevel = _mip_levels - 1;
        mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

        Vulkan::_EndCommandBufferSingleCommand(m_instance_creator.GetDevice(), m_instance_creator.GetGraphicsQueue(), pool, cmd_buf);
    }


    void VulkanRenderer::_CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels) {
        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = m_instance_creator.GetMaxSamplerAnisotropy();
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = static_cast<float>(_mip_levels);
        sampler_info.mipLodBias = 0.0f;

        if(vkCreateSampler(m_instance_creator.GetDevice(), &sampler_info, nullptr, &_tex.sampler) != VK_SUCCESS)
            VK_RES_ERR("Failed to create a texture sampler");
    }


    void VulkanRenderer::_Resize() {
        while (m_previous_canvas != m_conf.canvas_size) {
            m_previous_canvas = m_conf.canvas_size;
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }

        vkDeviceWaitIdle(m_instance_creator.GetDevice());

        Vulkan::Framebuffer &fb = *m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second;
        fb.DestroyFramebuffer();
        m_framebuffer_draws.find(MAIN_FRAMEBUFFER_NAME)->second.extent = { 
            m_conf.canvas_size.x,
            m_conf.canvas_size.y
        };

        m_swapchain_creator.RecreateSwapchain(m_conf.canvas_size);
        fb.NewFramebufferImages(m_swapchain_creator.GetSwapchainImages());
        fb.RecreateFramebuffer();

        vkDeviceWaitIdle(m_instance_creator.GetDevice());
    }
}
