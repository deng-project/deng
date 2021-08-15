/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: rend_init.cpp - Vulkan renderer initialiser class implementation file
/// author: Karl-Mihkel Ott


#define __VK_REND_INIT_CPP
#include <deng/vulkan/rend_init.h>


namespace deng {
    namespace vulkan {
        
        __vk_RendererInitialiser::__vk_RendererInitialiser (
            deng::Window &win, 
            const __vk_ConfigVars &conf,
            deng::Registry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures
        ) : m_reg(reg), m_assets(assets), m_textures(textures) {

            // Create new VkInstace
            m_ic = std::make_unique<__vk_InstanceCreator>(win, conf.enable_validation_layers);
            
            // Create new swapchain creator
            m_scc = std::make_unique<__vk_SwapChainCreator>(m_ic->getDev(),
                win, m_ic->getGpu(), m_ic->getSu(), m_ic->getSurfaceCapabilities(), m_ic->getQFF(),
                conf.msaa_sample_count, m_ic->getUserData());

            // Create new vulkan descriptor creator
            m_desc_c = std::make_unique<__vk_DescriptorSetsCreator>(m_ic->getDev(),
                m_scc->getExt(), m_scc->getRp(), m_reg, m_assets,
                m_textures, conf.msaa_sample_count, m_ic->getUserData());

            // Create new draw caller instance and make command pool
            m_dc = std::make_unique<__vk_DrawCaller>(m_ic->getDev(),
                m_ic->getQFF(), m_assets, m_textures, m_desc_c->getUIDS(), m_reg);
            
            m_dc->mkCommandPool(m_ic->getDev());

            __max_frame_c = static_cast<deng_ui32_t>(m_scc->getSCImg().size());

            // Create new buffer resources allocator
            m_rm = std::make_unique<__vk_ResourceManager>(m_ic->getDev(), 
                m_ic->getGpu(), m_scc->getExt(), conf.msaa_sample_count,
                m_scc->getRp(), m_dc->getComPool(), m_ic->getQFF().graphics_queue,
                m_scc->getSCImgViews(), m_reg, m_assets, m_textures, m_scc->getSF(),
                m_ic->getGpuLimits(), m_ic->getUserData()); 

            // Create new vulkan pipeline creator
            m_pl_c = std::make_unique<__vk_PipelineCreator>(
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UNMAPPED_2D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UNMAPPED_3D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UI)
            );
        }


        /// Renderer initialiser getter methods
        __vk_InstanceCreator &__vk_RendererInitialiser::getIC() { return *m_ic.get(); }
        __vk_SwapChainCreator &__vk_RendererInitialiser::getSCC() { return *m_scc.get(); } 
        __vk_DescriptorSetsCreator &__vk_RendererInitialiser::getDescC() { return *m_desc_c.get(); }
        __vk_PipelineCreator &__vk_RendererInitialiser::getPipelineC() { return *m_pl_c.get(); }
        __vk_ResourceManager &__vk_RendererInitialiser::getResMan() { return *m_rm.get(); }
        __vk_DrawCaller &__vk_RendererInitialiser::getDrawCaller() { return *m_dc.get(); }


        void __vk_RendererInitialiser::setUIDataPtr(__ImGuiData *p_gui) { 
            m_rm->setUIDataPtr(p_gui); 
            m_dc->setUIDataPtr(p_gui);
        }
    }
}
