/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.cpp - OpenGL uniform buffer manager parent class implementation
/// author: Karl-Mihkel Ott

#define __GL_UBM_CPP
#include <deng/opengl/ubm.h>


namespace deng {
    namespace opengl {

        __gl_UniformManager::__gl_UniformManager(Registry &reg, std::vector<deng_Id> &assets, __gl_Resources &res, void (*lgl_err_check)(const std::string &func_name, const std::string &file, const deng_ui32_t line)) 
            : __OffsetFinder(assets, reg), m_reg(reg), m_resources(res), lglErrorCheck(lgl_err_check) {
            glGenBuffers(1, &m_resources.ubo_buffer);

            LOG("new ubo_buffer value: " + std::to_string(m_resources.ubo_buffer));

            glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
            glErrorCheck("glBindBuffer", __FILE__,__LINE__);
            
            glBufferData(GL_UNIFORM_BUFFER, __OffsetFinder::getSectionInfo().ui_cap, NULL, GL_DYNAMIC_DRAW);  
            glErrorCheck("glBufferData", __FILE__,__LINE__);
        }


        /// Reserve a uniform data memory location for OpenGL asset
        void __gl_UniformManager::mapUniformBufferArea(das_Asset &asset) {
            asset.offsets.ubo_offset = __OffsetFinder::getSectionInfo().ubo_asset_size;

            // Check the asset mode for assgning correct asset data values
            if(asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                __OffsetFinder::getSectionInfo().ubo_asset_size += sizeof(__UniformAssetData2D);
            }

            else __OffsetFinder::getSectionInfo().ubo_asset_size += sizeof(__UniformAssetData);

            // Check if the uniform buffer needs to be reallocated
            if(__OffsetFinder::getSectionInfo().ubo_asset_size > __OffsetFinder::getSectionInfo().ubo_asset_cap) {
                const deng_ui64_t non_asset_ubo_asset_size = sizeof(__UniformObjectTransform) + sizeof(__UniformObjectTransform2D) + 
                    sizeof(__UniformLightData);

                // Set the asset ubo capacity
                __OffsetFinder::getSectionInfo().ubo_asset_cap = std::max(__OffsetFinder::getSectionInfo().ubo_asset_cap * 3 / 2, 
                    __OffsetFinder::getSectionInfo().ubo_asset_size * 3 / 2);

                // Set the overall ubo capacity
                __OffsetFinder::getSectionInfo().ubo_cap = __OffsetFinder::getSectionInfo().ubo_asset_cap + 
                    __OffsetFinder::getSectionInfo().ubo_non_asset_size;

                // Reallocate uniform buffers
                glBufferData(GL_UNIFORM_BUFFER, __OffsetFinder::getSectionInfo().ui_cap, NULL, GL_DYNAMIC_DRAW);
                glErrorCheck("glBufferData", __FILE__, __LINE__);
            }
        }


        /// Update uniform transformation data for the frame according to 
        /// camera's view and projection matrix
        void __gl_UniformManager::updateUboTransform3D(Camera3D *p_cam) {
            __UniformObjectTransform ubo;
            ubo.transform = p_cam->getCameraMat();
            ubo.view = p_cam->getViewMat();

            // 3D uniform transformation data always has binding of 0 and offset of 0
            glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_resources.ubo_buffer, 0, sizeof(__UniformObjectTransform));
        }


        /// Update asset uniform buffer data
        void __gl_UniformManager::updateAssetUboData(das_Asset &asset) {
            if(asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                __UniformAssetData ubo = {};

                // Set all asset lighting / material properties
                ubo.ambient = asset.ambient;
                ubo.diffuse = asset.diffuse;
                ubo.phong_exp = asset.phong_exp;

                // Additional asset shader flags
                ubo.ignore_transform = static_cast<deng_ui32_t>(asset.is_transformed);
                ubo.is_unmapped = static_cast<deng_ui32_t>(asset.force_unmap);

                // Asset uniform binding is always 1 and has offset of specified asset ubo_offset
                glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_resources.ubo_buffer, asset.offsets.ubo_offset, sizeof(__UniformAssetData));
            }

            else {
                __UniformAssetData2D ubo = {};
                ubo.color = asset.diffuse;
                ubo.is_transform = asset.is_transformed;
                ubo.is_unmapped = asset.force_unmap;

                // Asset uniform binding is always 1 and has offset of specified asset ubo_offset
                glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_resources.ubo_buffer, asset.offsets.ubo_offset, sizeof(__UniformAssetData2D));
            }
        }

        
        /// Update lighting uniform data
        void __gl_UniformManager::updateUboLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs) {
            __UniformLightData ubo = {};

            // For each light source add it to light source ubo structure
            for(deng_ui64_t i = 0; i < light_srcs.size(); i++) {
                // Quit if no light source id is specified
                if(!light_srcs[i]) break;

                deng_RegistryType rtype = {};
                deng_RegistryTypeBitMask mask_type = DENG_REGISTRY_TYPE_PT_LIGHT | DENG_REGISTRY_TYPE_SUN_LIGHT | 
                    DENG_REGISTRY_TYPE_DIR_LIGHT;

                // Retrieve the current light source from the registry
                RegData &reg_light = m_reg.retrieve(light_srcs[i], mask_type, &rtype);

                switch(rtype) {
                case DENG_REGISTRY_TYPE_PT_LIGHT:
                    ubo.light_srcs[i].intensity = reg_light.pt_light.intensity;
                    ubo.light_srcs[i].pos = reg_light.pt_light.pos;
                    ubo.light_src_c++;
                    break;

                default:
                    goto BIND;
                }
            }

            // Light source always has a binding of 2 and offset of sizeof(__UniformAssetData) + sizeof(__UniformAssetData2D)
            BIND:
            glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_resources.ubo_buffer, sizeof(__UniformAssetData) + 
                sizeof(__UniformAssetData2D), sizeof(__UniformLightData));
        }
    }
}
