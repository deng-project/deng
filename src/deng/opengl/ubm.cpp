/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.cpp - OpenGL uniform buffer manager parent class implementation
/// author: Karl-Mihkel Ott


#define __GL_UBM_CPP
#include <deng/opengl/ubm.h>


namespace deng {
    namespace opengl {

        __gl_UniformManager::__gl_UniformManager (
            Registry &reg, 
            std::vector<deng_Id> &assets, 
            PFNGLERRORCHECK lgl_error_check, 
            const std::vector<deng_ugl_t> &programs
        ) : __OffsetFinder(assets, reg), m_reg(reg), lglErrorCheck(lgl_error_check) {

            // Find the minimal buffer offset alignment
            GLint min_align;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &min_align);
            m_min_align = static_cast<deng_ui64_t>(min_align);

            deng::BufferSectionInfo &buf_sec = __OffsetFinder::getSectionInfo();
            buf_sec.ubo_asset_cap = DENG_DEFAULT_ASSET_CAP * cm_FindChunkSize(m_min_align, std::max(sizeof(__UniformAssetData), sizeof(__UniformAssetData2D))); 
            buf_sec.ubo_non_asset_size = m_const_offset = cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) + 
														  cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)) + 
														  cm_FindChunkSize(m_min_align, sizeof(__UniformLightData)); 
            buf_sec.ubo_cap = buf_sec.ubo_asset_cap + buf_sec.ubo_non_asset_size;
            m_global_ubo_chunk_size = buf_sec.ubo_non_asset_size;

            glGenBuffers(1, &m_resources.ubo_buffer);
            glErrorCheck("glGenBuffers");

            glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
            glErrorCheck("glBindBuffer", __FILE__,__LINE__);
            
            glBufferData(GL_UNIFORM_BUFFER, buf_sec.ubo_cap, NULL, GL_STATIC_DRAW);  
            glErrorCheck("glBufferData", __FILE__,__LINE__);

            LOG("Uniform buffer capacity is " + std::to_string(buf_sec.ubo_cap) + ", sizeof UniformObjectTransformation " + std::to_string(sizeof(__UniformObjectTransform)));

            GLint max_ubo;
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_ubo);
            glErrorCheck("glGetIntegerv");
            LOG("Maximum uniform block size " + std::to_string(max_ubo));

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            __bindUniformBlocks(programs);
        }


        __gl_UniformManager::~__gl_UniformManager() {
            LOG("Destroying __gl_UniformManager class");
        }


        void __gl_UniformManager::__bindUniformBlocks(const std::vector<deng_ugl_t> &programs) {
            // 2D unmapped uniform bindings
            __bindUniformBlockToBindingPoint("UniformData", UM2D_UNIFORMDATA_INDEX, programs[UM2D_I]);
            __bindUniformBlockToBindingPoint("AssetData", UM2D_ASSETDATA_INDEX, programs[UM2D_I]);

            // 2D texture mapped uniform bindings
            __bindUniformBlockToBindingPoint("UniformData", TM2D_UNIFORMDATA_INDEX, programs[TM2D_I]);
            __bindUniformBlockToBindingPoint("AssetData", TM2D_ASSETDATA_INDEX, programs[TM2D_I]);

            // 3D unmapped uniform bindings
            __bindUniformBlockToBindingPoint("UniformData", UM3D_UNIFORMDATA_INDEX, programs[UM3D_I]);
            __bindUniformBlockToBindingPoint("AssetData", UM3D_ASSETDATA_INDEX, programs[UM3D_I]);
            __bindUniformBlockToBindingPoint("LightData", UM3D_LIGHTDATA_INDEX, programs[UM3D_I]);
            
            // 3D texture mapped uniform bindings
            __bindUniformBlockToBindingPoint("UniformData", TM3D_UNIFORMDATA_INDEX, programs[TM3D_I]);
            // __bindUniformBlockToBindingPoint("AssetData", TM3D_ASSETDATA_INDEX, programs[TM3D_I]);
            // __bindUniformBlockToBindingPoint("LightData", TM3D_LIGHTDATA_INDEX, programs[TM3D_I]);
        }


        void __gl_UniformManager::__bindUniformBlockToBindingPoint(const std::string &block_name, const deng_ugl_t bpt, const deng_ugl_t program) {
            deng_ui32_t index = glGetUniformBlockIndex(program, "UniformData");
            glErrorCheck("glGetUniformBlockIndex");

            glUniformBlockBinding(program, index, bpt);
            glErrorCheck("glUniformBlockBinding");
        }


        /// Reserve a uniform data memory location for OpenGL asset
        void __gl_UniformManager::mapUniformBufferArea(das_Asset &asset) {
            asset.offsets.ubo_offset = __OffsetFinder::getSectionInfo().ubo_asset_size;

            // Check the asset mode for assgning correct asset data values
            if(asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                __OffsetFinder::getSectionInfo().ubo_asset_size += cm_FindChunkSize(m_min_align, sizeof(__UniformAssetData));
            }

            else __OffsetFinder::getSectionInfo().ubo_asset_size += cm_FindChunkSize(m_min_align, sizeof(__UniformAssetData2D));

            // Check if the uniform buffer needs to be reallocated
            if(__OffsetFinder::getSectionInfo().ubo_asset_size > __OffsetFinder::getSectionInfo().ubo_asset_cap) {
                // Set the asset ubo capacity
                __OffsetFinder::getSectionInfo().ubo_asset_cap = std::max(__OffsetFinder::getSectionInfo().ubo_asset_cap * 3 / 2, 
                    __OffsetFinder::getSectionInfo().ubo_asset_size * 3 / 2);

                // Set the overall ubo capacity
                __OffsetFinder::getSectionInfo().ubo_cap = __OffsetFinder::getSectionInfo().ubo_asset_cap + 
                    __OffsetFinder::getSectionInfo().ubo_non_asset_size;

                glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
                glErrorCheck("glBindBuffer");

                // Reallocate uniform buffers
                glBufferData(GL_UNIFORM_BUFFER, __OffsetFinder::getSectionInfo().ubo_cap, NULL, GL_STATIC_DRAW);
                glErrorCheck("glBufferData");

                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                glErrorCheck("glBindBuffer");
            }

            updateAssetUboData(asset);
        }


        /// Update uniform transformation data for the frame according to 
        /// camera's view and projection matrix
        void __gl_UniformManager::updateUboTransform3D(Camera3D *p_cam) {
            __UniformObjectTransform ubo;
            ubo.transform = p_cam->getCameraMat();
            ubo.view = p_cam->getViewMat();
            // ubo.transform = dengMath::mat4<deng_vec_t>();
            // ubo.view = dengMath::mat4<deng_vec_t>();

            // 3D uniform transformation data always has binding of 0 and offset of 0
            glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
            glErrorCheck("glBindBuffer");

            void *data = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(__UniformObjectTransform), GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            memcpy(data, &ubo, sizeof(__UniformObjectTransform));
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glErrorCheck("glUnmapBuffer");

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glErrorCheck("glBindBuffer");
        }


        /// Update asset uniform buffer data
        void __gl_UniformManager::updateAssetUboData(const das_Asset &asset) {
            if(asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                __UniformAssetData ubo = {};

                // Set all asset lighting / material properties
                ubo.ambient = asset.ambient;
                ubo.diffuse = asset.diffuse;
                ubo.phong_exp = asset.phong_exp;

                // Additional asset shader flags
                ubo.ignore_transform = static_cast<deng_ui32_t>(asset.ignore_transform);
                ubo.is_unmapped = static_cast<deng_ui32_t>(asset.force_unmap);

                glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
                glErrorCheck("glBindBuffer");

                void *data = glMapBufferRange(GL_UNIFORM_BUFFER, m_const_offset + asset.offsets.ubo_offset, sizeof(__UniformAssetData), GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, &ubo, sizeof(__UniformAssetData));
                glUnmapBuffer(GL_UNIFORM_BUFFER);
                glErrorCheck("glUnmappBuffer");

                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                glErrorCheck("glBindBuffer");
            }

            else {
                __UniformAssetData2D ubo = {};
                ubo.color = asset.diffuse;
                ubo.ignore_transform = asset.ignore_transform;
                ubo.is_unmapped = asset.force_unmap;

                glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
                glErrorCheck("glBindBuffer");
                
                void* data = glMapBufferRange(GL_UNIFORM_BUFFER, m_const_offset + asset.offsets.ubo_offset, sizeof(__UniformAssetData2D), GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, &ubo, sizeof(__UniformAssetData2D));
                glUnmapBuffer(GL_UNIFORM_BUFFER);
                glErrorCheck("glUnmapBuffer");

                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                glErrorCheck("glBindBuffer");
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
                    goto MAP;
                }
            }

            // Light source always has a binding of 2 and offset of sizeof(__UniformObjectTransform) + sizeof(__UniformObjectTransform2D)
            MAP:
            const deng_ui64_t offset = cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) +
                                       cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D));

            glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
            glErrorCheck("glBindBuffer");

            void *data = glMapBufferRange(GL_UNIFORM_BUFFER, offset, sizeof(__UniformLightData), GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            memcpy(data, &ubo, sizeof(__UniformLightData));
            glUnmapBuffer(GL_UNIFORM_BUFFER);
            glErrorCheck("glUnmapBuffer");


            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }


        void __gl_UniformManager::bindAssetUboBufferRange(const das_Asset &asset) {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glErrorCheck("glBindBuffer");

            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                glBindBufferRange(GL_UNIFORM_BUFFER, UM2D_UNIFORMDATA_INDEX, m_resources.ubo_buffer, 
                                  cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)), sizeof(__UniformObjectTransform2D));
                glErrorCheck("glBindBufferRange");


                glBindBufferRange(GL_UNIFORM_BUFFER, UM2D_ASSETDATA_INDEX, m_resources.ubo_buffer, asset.offsets.ubo_offset, sizeof(__UniformAssetData2D));
                glErrorCheck("glBindBufferRange");
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                glBindBufferRange(GL_UNIFORM_BUFFER, TM2D_UNIFORMDATA_INDEX, m_resources.ubo_buffer, 
                                  cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)), sizeof(__UniformObjectTransform2D));
                glErrorCheck("glBindBufferRange");


                glBindBufferRange(GL_UNIFORM_BUFFER, TM2D_ASSETDATA_INDEX, m_resources.ubo_buffer, asset.offsets.ubo_offset, sizeof(__UniformAssetData2D));
                glErrorCheck("glBindBufferRange");
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                glBindBufferRange(GL_UNIFORM_BUFFER, UM3D_UNIFORMDATA_INDEX, m_resources.ubo_buffer, 0, sizeof(__UniformObjectTransform));
                glErrorCheck("glBindBufferRange");


                glBindBufferRange(GL_UNIFORM_BUFFER, UM3D_ASSETDATA_INDEX, m_resources.ubo_buffer, asset.offsets.ubo_offset, sizeof(__UniformAssetData));
                glErrorCheck("glBindBufferRange");

                glBindBufferRange(GL_UNIFORM_BUFFER, UM3D_LIGHTDATA_INDEX, m_resources.ubo_buffer, 
                                  cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) + cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)),
                                  sizeof(__UniformLightData));
                glErrorCheck("glBindBufferRange");
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_resources.ubo_buffer, 0, sizeof(__UniformObjectTransform));
                glErrorCheck("glBindBufferRange");

                // glBindBufferRange(GL_UNIFORM_BUFFER, TM3D_ASSETDATA_INDEX, m_resources.ubo_buffer, m_const_offset + asset.offsets.ubo_offset, sizeof(__UniformAssetData));
                // glErrorCheck("glBindBufferRange");

                // glBindBufferRange(GL_UNIFORM_BUFFER, TM3D_LIGHTDATA_INDEX, m_resources.ubo_buffer, 
                //                   cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform)) + cm_FindChunkSize(m_min_align, sizeof(__UniformObjectTransform2D)),
                //                   sizeof(__UniformLightData));
                // glErrorCheck("glBindBufferRange");
                break;

            default:
                break;
            }
        }


        __gl_Resources &__gl_UniformManager::getResources() {
            return m_resources;
        }
    }
}
