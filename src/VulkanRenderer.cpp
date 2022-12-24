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
        m_instance_creator(m_conf)
    {
        m_previous_canvas = m_conf.canvas_size;

        // push a missing 2D texture
        TextureDatabase* db = TextureDatabase::GetInstance();
        int x, y, size;
        char* data = GetMissingTextureRGBA(x, y, size);
        m_missing_2d = db->AddResource(data, (uint32_t)x, (uint32_t)y, 4);
        _CreateVendorTextureImages(m_missing_2d);


        // push a missing 3D texture
        m_missing_3d = db->AddResource((uint32_t)x, (uint32_t)y, 4, data, data, data, data, data, data);
        _CreateVendorTextureImages(m_missing_3d);

        // create main swapchain framebuffer
        VkExtent2D ext = m_instance_creator.GetSurfaceCapabilities().currentExtent;
        _AllocateBufferResources();

        m_framebuffers.emplace_back(new Vulkan::Framebuffer(
            m_instance_creator,
            m_uniform_buffer,
            m_main_buffer,
            m_sample_count,
            TRS::Point2D<uint32_t>(ext.width, ext.height),
            m_missing_2d,
            m_missing_3d
        ));
    }


    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(m_instance_creator.GetDevice());

        // destroy all secondary framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end();) {
            it = m_framebuffers.erase(it);
        }

        TextureDatabase* db = TextureDatabase::GetInstance();
        db->DeleteAll();

        // destroy texture resources
        while(!db->GetDeletedEventQueue().empty()) {
            TextureResource res = db->GetDeletedEventQueue().front();
            Vulkan::TextureData data = std::get<Vulkan::TextureData>(res.vendor);
            db->GetDeletedEventQueue().pop();

            if (res.resource_type == TEXTURE_RESOURCE_1D_IMAGE || res.resource_type == TEXTURE_RESOURCE_2D_IMAGE || res.resource_type == TEXTURE_RESOURCE_3D_IMAGE) {
                vkDestroySampler(m_instance_creator.GetDevice(), data.sampler, nullptr);
                vkDestroyImageView(m_instance_creator.GetDevice(), data.image_view, nullptr);
                vkDestroyImage(m_instance_creator.GetDevice(), data.image, nullptr);
                vkFreeMemory(m_instance_creator.GetDevice(), data.memory, nullptr);

                if (res.rgba_data) delete[] res.rgba_data;
            }
        }

        // free main buffers
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_main_buffer, NULL);
        vkFreeMemory(m_instance_creator.GetDevice(), m_main_memory, NULL);

        // free uniform buffers
        vkDestroyBuffer(m_instance_creator.GetDevice(), m_uniform_buffer, nullptr);
        vkFreeMemory(m_instance_creator.GetDevice(), m_uniform_memory, nullptr);
    }


    FramebufferIndices VulkanRenderer::AddFramebuffer(TRS::Point2D<uint32_t> _extent) {
        m_framebuffers.emplace_back(new Vulkan::Framebuffer(
            m_instance_creator, 
            m_uniform_buffer, 
            m_main_buffer, 
            m_sample_count, 
            _extent,
            m_missing_2d,
            m_missing_3d,
            true
        ));

        return FramebufferIndices(
            m_framebuffers.back()->GetFramebufferImageIds().front(), 
            static_cast<uint32_t>(m_framebuffers.size() - 1)
        );
    }


    uint32_t VulkanRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, m_instance_creator.GetMinimalUniformBufferAlignment());
    }


    void VulkanRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_size + _offset) >= m_uniform_size) {
            m_uniform_size = (_size + _offset) * 3 / 2;
            _ReallocateUniformBuffer();

            for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
                if (*it != nullptr) {
                    (*it)->RecreateDescriptorSets();
                }
            }
        }

        if(_raw_data) {
            Vulkan::_CopyToBufferMemory(m_instance_creator.GetDevice(), static_cast<VkDeviceSize>(_size), _raw_data, m_uniform_memory, static_cast<VkDeviceSize>(_offset));
        }
    }


    void VulkanRenderer::UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        const VkCommandPool pool = m_framebuffers[0]->GetCommandPool();

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
        auto swpchn_fb = m_framebuffers.begin();
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            if (*it == nullptr || (*it)->GetSwapchainBit())
                continue;

            if((*it)->GetSwapchainBit())
                continue;

            (*it)->ClearFrame();
        }

        (*swpchn_fb)->ClearFrame();
    }


    void VulkanRenderer::RenderFrame() {
        _CheckAndCopyTextures();
        _CheckAndDeleteTextures();
        _CheckAndDeleteMeshes();
        _CheckAndRemoveShaders();

        // acquire next images to draw
        VkSemaphore swapchain_image_semaphore = m_framebuffers[0]->GetSwapchainImageAcquisitionSemaphore();
        uint32_t imgi;
        VkResult res = vkAcquireNextImageKHR(
            m_instance_creator.GetDevice(), 
            m_framebuffers[0]->GetSwapchainCreator()->GetSwapchain(), 
            UINT64_MAX, 
            swapchain_image_semaphore, 
            VK_NULL_HANDLE, 
            &imgi);

        if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
            _Resize();
            return;
        } else if(res != VK_SUCCESS) {
            VK_FRAME_ERR("Failed to acquire next swapchain image");
        }

        
        // draw each mesh
        for (auto mesh_it = m_meshes.begin(); mesh_it != m_meshes.end(); mesh_it++) {
            if (*mesh_it) {
                for (auto fb_it = (*mesh_it)->framebuffer_ids.begin(); fb_it != (*mesh_it)->framebuffer_ids.end(); fb_it++) {
                    if (!m_framebuffers[*fb_it]) {
                        throw std::runtime_error("Cannot draw to framebuffer " + std::to_string(fb_it - (*mesh_it)->framebuffer_ids.begin()) + ". Framebuffer deleted.");
                    }

                    if (!m_framebuffers[*fb_it]->GetCommandBufferRecordingBit())
                        m_framebuffers[*fb_it]->StartCommandBufferRecording(m_conf.clear_color);
                    
                    m_framebuffers[*fb_it]->Draw(**mesh_it, static_cast<uint32_t>(mesh_it - m_meshes.begin()), m_shader_modules);
                }
            }
        }

        // end command buffer recordings for each framebuffer that got draw commands submitted
        for (size_t i = 0; i < m_framebuffers.size(); i++) {
            if (!m_framebuffers[i])
                continue;

            if (m_framebuffers[i]->GetCommandBufferRecordingBit()) {
                m_framebuffers[i]->EndCommandBufferRecording();
                m_framebuffers[i]->Render();
            }
        }

        // present the swapchain framebuffer
       res = m_framebuffers[0]->Present();
 
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
        const VkCommandPool pool = m_framebuffers[0]->GetCommandPool();

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
        const VkCommandPool pool = m_framebuffers[0]->GetCommandPool();
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


    void VulkanRenderer::_Resize() {
        while (m_previous_canvas != m_conf.canvas_size) {
            m_previous_canvas = m_conf.canvas_size;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        vkDeviceWaitIdle(m_instance_creator.GetDevice());

        Vulkan::Framebuffer &fb = *m_framebuffers[0];
        fb.DestroyFramebuffer();
        fb.SetExtent(m_conf.canvas_size);

        fb.GetSwapchainCreator()->RecreateSwapchain(m_conf.canvas_size);
        fb.RecreateFramebuffer();

        vkDeviceWaitIdle(m_instance_creator.GetDevice());
    }
}
