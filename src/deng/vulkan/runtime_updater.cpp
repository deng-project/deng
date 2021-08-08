/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: runtime_updater.h - Vulkan runtime buffer data update handler class implementation
/// author: Karl-Mihkel Ott


#define __VK_RUNTIME_UPDATE_CPP
#include <deng/vulkan/runtime_updater.h>


namespace deng {
    namespace vulkan {

        __vk_RuntimeUpdater::__vk_RuntimeUpdater (
            __vk_InstanceCreator &ic,
            __vk_SwapChainCreator &scc,
            __vk_DrawCaller &dc,
            __vk_ResourceManager &rm,
            __vk_DescriptorSetsCreator &desc_c,
            __GlobalRegistry &reg,
            std::vector<deng_Id> &assets, 
            std::vector<deng_Id> &tex
        ) : m_ic(ic), m_scc(scc), m_dc(dc), m_rm(rm), m_desc_c(desc_c), m_reg(reg), 
            m_assets(assets), m_tex(tex) {}

        
        /// Rerecord new level one commandbuffers if possible
        void __vk_RuntimeUpdater::updateCmdBuffers(const dengMath::vec4<deng_vec_t> &background) {
            vkWaitForFences(m_ic.getDev(), static_cast<deng_ui32_t>(m_dc.flight_fences.size()),
                m_dc.flight_fences.data(), VK_TRUE, UINT64_MAX);

            // Record new commandbuffers
            m_dc.recordCmdBuffers(m_scc.getRp(), m_scc.getExt(),
                background, m_rm.getBD(), m_rm.getSectionInfo());
        }


        /// This method updates the vertices buffer that is allocated by given assets
        void __vk_RuntimeUpdater::updateAssetData(const dengMath::vec2<deng_ui32_t> &bounds) {
            // Check if reallocation is needed
            const deng_bool_t is_realloc = m_rm.reallocCheck(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(), 
                m_ic.getQFF().graphics_queue, m_dc.flight_fences);

            if(!is_realloc) {
                m_rm.cpyAssetsToBuffer(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(), 
                    m_ic.getQFF().graphics_queue, false, bounds);
            }
        }


        /// Update ImGui vertices and indices
        void __vk_RuntimeUpdater::updateUIData(const dengMath::vec4<deng_vec_t> &background) {
            // Check if reallocation is needed
            const deng_bool_t is_realloc = m_rm.reallocCheck(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(), 
                m_ic.getQFF().graphics_queue, m_dc.flight_fences);

            //// Copy UI element data to buffer, if no reallocation occured
            if(!is_realloc) {
                m_rm.cpyUIDataToBuffer(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(),
                    m_ic.getQFF().graphics_queue, false);
            }

            // Update command buffers for remapped ui elements
            updateCmdBuffers(background);
        }


        /// Reallocate or allocate new descriptor sets for assets between given bounds
        /// NOTE: Vulkan renderer must be idled
        void __vk_RuntimeUpdater::updateAssetDS(const dengMath::vec2<deng_ui32_t> &bounds) {
            // For each asset between bounds, check its descriptor set status
            for(deng_ui32_t i = bounds.first; i < bounds.second; i++) {
                // Retrieve base and Vulkan assets
                RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);
                RegType &reg_vk_asset = m_reg.retrieve(reg_asset.asset.vk_id, DENG_SUPPORTED_REG_TYPE_VK_ASSET, NULL);

                // Check if descriptor sets have been previously allocated
                if(reg_vk_asset.vk_asset.is_desc) {
                    vkFreeDescriptorSets(m_ic.getDev(), m_desc_c.getDescPool(assetModeToPipelineType(reg_asset.asset.asset_mode)), 
                        reg_vk_asset.vk_asset.desc_c, reg_vk_asset.vk_asset.desc_sets);
                }

                else reg_vk_asset.vk_asset.is_desc = true;

                m_desc_c.mkAssetDS(m_ic.getDev(), m_rm.getBD(), m_rm.getMissingTextureUUID(), bounds, 
                    m_rm.getUboChunkSize(), m_ic.getGpuLimits().minUniformBufferOffsetAlignment);
            }
        }


        /// Update texture texel data, however it does not perform any texture size adjustments. For that
        /// consider creating a new texture
        void __vk_RuntimeUpdater::updateTexelData(__vk_Texture &tex) {
            m_rm.cpyBitmap(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(), m_ic.getQFF().graphics_queue, 
                true, tex);
        }

        
        /// Reallocate main buffer and copy all asset to it
        /// NOTE: Vulkan renderer must be idled
        void __vk_RuntimeUpdater::checkForReallocation() {
            m_rm.reallocCheck(m_ic.getDev(), m_ic.getGpu(), m_dc.getComPool(), m_ic.getQFF().graphics_queue, 
                m_dc.flight_fences);
        }
    }
}
