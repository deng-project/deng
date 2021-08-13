/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: desc_sets.cpp - Vulkan descriptor set creator class implementation
/// author: Karl-Mihkel Ott


#define __VK_DESC_SETS_CPP
#include <deng/vulkan/desc_sets.h>

namespace deng {
    namespace vulkan {

        /********************************************************/
        /********** __vk_DescriptorSetsCreator class ************/
        /********************************************************/

        __vk_DescriptorSetsCreator::__vk_DescriptorSetsCreator (
            VkDevice device, 
            VkExtent2D extent, 
            VkRenderPass renderpass, 
            deng::Registry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures,
            VkSampleCountFlagBits sample_c
        ) : __vk_DescriptorPoolCreator(assets, reg), 
            __vk_DescriptorSetLayoutCreator(device), m_assets(assets), 
            m_textures(textures), m_reg(reg) {

            // Create the initial descriptor pools
            __vk_DescriptorPoolCreator::mkDescPools(device, __DEFAULT_DESC_POOL_CAP);
        }

        
        /// Abstracted function for creating descriptor sets for assets
        /// for assets between the asset bounds
        void __vk_DescriptorSetsCreator::mkAssetDS (
            VkDevice device,
            __vk_BufferData &bd,
            deng_Id missing_tex_uuid,
            const dengMath::vec2<deng_ui32_t> &asset_bounds,
            deng_ui64_t ubo_chunk_size,
            deng_ui64_t min_ubo_align
        ) {
            LOG("Asset bounds: " + std::to_string(asset_bounds.first) + " " + std::to_string(asset_bounds.second));
            // Count new added assets by their type
            for(size_t i = asset_bounds.first; i < asset_bounds.second; i++) {
                // Retrieve base asset
                RegData &reg_asset = m_reg.retrieve(
                    m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);

                // Retrieve vulkan asset
                RegData &reg_vk_asset = m_reg.retrieve (
                    reg_asset.asset.vk_id, DENG_REGISTRY_TYPE_VK_ASSET, NULL);

                // Increment asset count accordingly
                switch(reg_asset.asset.asset_mode) {
                case DAS_ASSET_MODE_2D_UNMAPPED:
                    m_2d_unmapped_asset_c++;
                    break;

                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                    m_2d_mapped_asset_c++;
                    break;

                case DAS_ASSET_MODE_3D_UNMAPPED:
                    m_3d_unmapped_asset_c++;
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    m_3d_mapped_asset_c++;
                    break;

                default:
                    // Throw runtime error, because asset mode given was invalid
                    RUN_ERR("__vk_DescriptorSetsCreator::mkDS()", 
                        "Invalid given asset mode");
                    break;
                }
            }

            // Check if descriptor pools need to be reallocated
            std::vector<deng_Id> destroyed_assets;
            destroyed_assets.reserve(m_assets.size());
            __vk_DescriptorPoolCreator::checkPoolReallocation(device, 
                destroyed_assets, m_2d_unmapped_asset_c, m_2d_mapped_asset_c,
                m_3d_unmapped_asset_c, m_3d_mapped_asset_c);

            // Create new descriptor sets for all assets that are between given bounds
            for(size_t i = asset_bounds.first; i < asset_bounds.second; i++) {
                RegData &reg_asset = m_reg.retrieve(m_assets[i],
                    DENG_REGISTRY_TYPE_ASSET, NULL);

                RegData &reg_vk_asset = m_reg.retrieve(reg_asset.asset.vk_id,
                    DENG_REGISTRY_TYPE_VK_ASSET, NULL);

                // Create texture mapped descriptor sets
                if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED ||
                   reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                    __mkTexMappedDS(device, reg_vk_asset.vk_asset,
                        bd, missing_tex_uuid, min_ubo_align, ubo_chunk_size);
                }

                // Create non-texture mapped descriptor sets
                else if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED ||
                        reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                    __mkUnmappedDS(device, reg_vk_asset.vk_asset,
                        bd, min_ubo_align, ubo_chunk_size);
                }
            }
        }


        /// Check if the pool was reallocated and if it was reallocate 
        /// descriptor sets for each asset that had them destroyed
        void __vk_DescriptorSetsCreator::__reallocCheck (
            VkDevice device,
            __vk_BufferData &bd,
            deng_Id non_tex_id,
            deng_ui64_t min_ubo_align,
            deng_ui64_t ubo_chunk_size
        ) {
            // Create a vector to store the ids of all assets that had their
            // descriptor sets destroyed
            std::vector<deng_Id> destroyed_assets;
            destroyed_assets.reserve(m_assets.size());
            __vk_DescriptorPoolCreator::checkPoolReallocation(device,
                destroyed_assets, m_2d_unmapped_asset_c, m_2d_mapped_asset_c,
                m_3d_unmapped_asset_c, m_3d_mapped_asset_c);

            // For each affected asset create new descriptor sets
            for(size_t i = 0; i < destroyed_assets.size(); i++) {
                // Retrieve the Vulkan asset
                RegData &reg_vk_asset = m_reg.retrieve(destroyed_assets[i],
                    DENG_REGISTRY_TYPE_VK_ASSET, NULL);

                // Retrieve the base asset
                RegData &reg_asset = m_reg.retrieve(reg_vk_asset.vk_asset.base_id,
                    DENG_REGISTRY_TYPE_ASSET, NULL);

                // Choose the descriptor set allocation method according to the asset mode
                switch(reg_asset.asset.asset_mode) {
                case DAS_ASSET_MODE_2D_UNMAPPED:
                case DAS_ASSET_MODE_3D_UNMAPPED:
                    __mkUnmappedDS(device, reg_vk_asset.vk_asset, bd, min_ubo_align, ubo_chunk_size);
                    break;

                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    __mkTexMappedDS(device, reg_vk_asset.vk_asset, bd, non_tex_id,
                        min_ubo_align, ubo_chunk_size);
                    break;

                default:
                    break;
                }
            }
        }



        /// Find bufferinfos based on the asset mode used
        std::vector<VkDescriptorBufferInfo> __vk_DescriptorSetsCreator::__findBufferInfos(
            das_Asset &asset,
            __vk_BufferData &bd,
            deng_ui64_t cur_frame,
            deng_ui64_t frame_c,
            deng_ui64_t min_align,
            deng_ui64_t ubo_chunk_size
        ) {
            std::vector<VkDescriptorBufferInfo> buf_info{};

            // Check the given asset mode and set the values accordingly
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                buf_info.resize(2);
                buf_info[0].buffer = bd.uniform_buffer;
                buf_info[0].offset = cur_frame * ubo_chunk_size + cm_FindChunkSize(min_align, sizeof(__UniformObjectTransform));
                buf_info[0].range = sizeof(__UniformObjectTransform2D);

                buf_info[1].buffer = bd.uniform_buffer;
                buf_info[1].offset = asset.offsets.ubo_offset + cur_frame * cm_FindChunkSize(min_align, sizeof(__UniformAssetData));
                buf_info[1].range = sizeof(__UniformAssetData);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                buf_info.resize(3);
                buf_info[0].buffer = bd.uniform_buffer;
                buf_info[0].offset = cur_frame * ubo_chunk_size;
                buf_info[0].range = sizeof(__UniformObjectTransform);

                buf_info[1].buffer = bd.uniform_buffer;
                buf_info[1].offset = asset.offsets.ubo_offset + cur_frame * cm_FindChunkSize(min_align, sizeof(__UniformAssetData));
                buf_info[1].range = sizeof(__UniformAssetData);

                buf_info[2].buffer = bd.uniform_buffer;
                buf_info[2].offset = cur_frame * ubo_chunk_size + cm_FindChunkSize(min_align, sizeof(__UniformObjectTransform)) +
                    cm_FindChunkSize(min_align, sizeof(__UniformObjectTransform2D));
                buf_info[2].range = sizeof(__UniformLightData);
                break;

            default:
                RUN_ERR("__vk_DescriptorSetsCreator::__findBufferInfo()", 
                    "Invalid asset mode for asset: " + std::string(asset.uuid));
                break;
            }

            return buf_info;
        }


        /// Create write descriptors for texture mapped assets
        const std::vector<VkWriteDescriptorSet> __vk_DescriptorSetsCreator::__mkMappedWriteDescInfos (
            das_AssetMode asset_mode, 
            std::vector<VkDescriptorBufferInfo> &buffer_info, 
            VkDescriptorSet &desc_set,
            VkDescriptorImageInfo &img_info
        ) {
            std::vector<VkWriteDescriptorSet> write_descriptors(2);
            // Transformation uniform data
            write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptors[0].dstSet = desc_set;
            write_descriptors[0].dstBinding = 0;
            write_descriptors[0].dstArrayElement = 0;
            write_descriptors[0].descriptorCount = 1;
            write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptors[0].pBufferInfo = &buffer_info[0];

            // Color uniform data
            write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptors[1].dstSet = desc_set;
            write_descriptors[1].dstBinding = 1;
            write_descriptors[1].dstArrayElement = 0;
            write_descriptors[1].descriptorCount = 1;
            write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptors[1].pBufferInfo = &buffer_info[1];


            // Check the current asset mode in order to set correct write descriptors
            switch(asset_mode) {
            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                write_descriptors.resize(3);

                // Set texture sampler
                write_descriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_descriptors[2].dstSet = desc_set;
                write_descriptors[2].dstBinding = 2;
                write_descriptors[2].dstArrayElement = 0;
                write_descriptors[2].descriptorCount = 1;
                write_descriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_descriptors[2].pImageInfo = &img_info;
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                write_descriptors.resize(4);

                // Set lighting data
                write_descriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_descriptors[2].dstSet = desc_set;
                write_descriptors[2].dstBinding = 2;
                write_descriptors[2].dstArrayElement = 0;
                write_descriptors[2].descriptorCount = 1;
                write_descriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write_descriptors[2].pBufferInfo = &buffer_info[2];
                
                // Set texture sampler
                write_descriptors[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_descriptors[3].dstSet = desc_set;
                write_descriptors[3].dstBinding = 3;
                write_descriptors[3].dstArrayElement = 0;
                write_descriptors[3].descriptorCount = 1;
                write_descriptors[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write_descriptors[3].pImageInfo = &img_info;
                break;

            default:
                break;
            }

            return write_descriptors;
        }


        /// Create write descriptors for unmapped assets
        const std::vector<VkWriteDescriptorSet> __vk_DescriptorSetsCreator::__mkUnmappedWriteDescInfos(
            das_AssetMode asset_mode,
            std::vector<VkDescriptorBufferInfo> &buffer_info,
            VkDescriptorSet &desc_set
        ) {
            std::vector<VkWriteDescriptorSet> write_descriptors(2);
            // Transformation uniform data
            write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptors[0].dstSet = desc_set;
            write_descriptors[0].dstBinding = 0;
            write_descriptors[0].dstArrayElement = 0;
            write_descriptors[0].descriptorCount = 1;
            write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptors[0].pBufferInfo = &buffer_info[0];

            // Color uniform data
            write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptors[1].dstSet = desc_set;
            write_descriptors[1].dstBinding = 1;
            write_descriptors[1].dstArrayElement = 0;
            write_descriptors[1].descriptorCount = 1;
            write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptors[1].pBufferInfo = &buffer_info[1];

            // Check if the unmapped type is for 3D asset type
            if(asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                write_descriptors.resize(3);
                // Set lighting data
                write_descriptors[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write_descriptors[2].dstSet = desc_set;
                write_descriptors[2].dstBinding = 2;
                write_descriptors[2].dstArrayElement = 0;
                write_descriptors[2].descriptorCount = 1;
                write_descriptors[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write_descriptors[2].pBufferInfo = &buffer_info[2];
            }

            return write_descriptors;
        }


        /// Create write descriptors for ImGui elements
        const VkWriteDescriptorSet __vk_DescriptorSetsCreator::__mkUIWriteDescInfo (
            const VkDescriptorSet &desc_set,
            const VkDescriptorImageInfo &img_info
        ) {
            VkWriteDescriptorSet write_desc_set = {};
            write_desc_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_desc_set.dstSet = desc_set;
            write_desc_set.dstBinding = 0;
            write_desc_set.dstArrayElement = 0;
            write_desc_set.descriptorCount = 1;
            write_desc_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write_desc_set.pImageInfo = &img_info;

            return write_desc_set;
        }


        /// Create unmapped descriptor sets for a single asset
        void __vk_DescriptorSetsCreator::__mkUnmappedDS (
            VkDevice device, 
            __vk_Asset &asset,
            __vk_BufferData &bd,
            deng_ui64_t min_align,
            deng_ui64_t ubo_chunk_size
        ) {
            // Set up multiple descriptor layout structures
            std::vector<VkDescriptorSetLayout> set_layouts;
            set_layouts.resize(__max_frame_c);

            // Retrieve the base asset from registry
            RegData &reg_asset = m_reg.retrieve(asset.base_id, 
                DENG_REGISTRY_TYPE_ASSET, NULL);

            // Allocate memory for descriptor sets
            asset.desc_c = __max_frame_c;
            asset.desc_sets = (VkDescriptorSet*) calloc (
                asset.desc_c, sizeof(VkDescriptorSet));

            // Set all layouts per swapchain image
            for(size_t i = 0; i < set_layouts.size(); i++)
                set_layouts[i] = __vk_DescriptorSetLayoutCreator::getLayout(assetModeToPipelineType(reg_asset.asset.asset_mode));
            
            // Create descriptor set allocate info instance
            VkDescriptorSetAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocinfo.descriptorPool = __vk_DescriptorPoolCreator::getDescPool(assetModeToPipelineType(reg_asset.asset.asset_mode));
            allocinfo.descriptorSetCount = static_cast<deng_ui32_t>(set_layouts.size());
            allocinfo.pSetLayouts = set_layouts.data();

            // Allocate all required descriptor sets 
            if(vkAllocateDescriptorSets(device, &allocinfo, asset.desc_sets) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets for unmapped assets");

            // For each image create descriptor sets
            for(size_t i = 0; i < __max_frame_c; i++) {
                // Find required bufferinfos
                std::vector<VkDescriptorBufferInfo> buffer_infos = __findBufferInfos(reg_asset.asset,
                    bd, i, __max_frame_c, min_align, ubo_chunk_size);

                // Find required write descriptors
                std::vector<VkWriteDescriptorSet> write_desc = __mkUnmappedWriteDescInfos(reg_asset.asset.asset_mode,
                    buffer_infos, asset.desc_sets[i]);

                // Update descriptor sets
                vkUpdateDescriptorSets(device, static_cast<deng_ui32_t>(write_desc.size()), 
                    write_desc.data(), 0, NULL);
            }
        }
        

        /// Create texture mapped descriptor sets for single asset
        void __vk_DescriptorSetsCreator::__mkTexMappedDS (
            VkDevice device, 
            __vk_Asset &asset,
            __vk_BufferData &bd,
            deng_Id missing_tex_uuid,
            deng_ui64_t min_align,
            deng_ui64_t ubo_chunk_size
        ) {
            std::vector<VkDescriptorSetLayout> set_layouts;
            set_layouts.resize(__max_frame_c);

            // Retrieve the base asset
            RegData &reg_asset = m_reg.retrieve(asset.base_id,
                DENG_REGISTRY_TYPE_ASSET, NULL);

            // Texture entry pointer that is going to have an appropriate value according to the given texture id
            RegData *p_reg_vk_tex = NULL;

            // Retrieve the asset texture if it exists, otherwise retrieve missing texture texture
            if(reg_asset.asset.tex_uuid) {
                // Retrieve the base texture and then retrieve Vulkan texture
                RegData reg_tex = m_reg.retrieve(reg_asset.asset.tex_uuid,
                    DENG_REGISTRY_TYPE_TEXTURE, NULL);

                p_reg_vk_tex = m_reg.retrievePtr(reg_tex.tex.vk_id, 
                    DENG_REGISTRY_TYPE_VK_TEXTURE, NULL);
            }

            else {
                // Retrieve the base texture and then retrieve Vulkan texture
                RegData reg_tex = m_reg.retrieve(missing_tex_uuid,
                    DENG_REGISTRY_TYPE_TEXTURE, NULL);

                p_reg_vk_tex = m_reg.retrievePtr(reg_tex.tex.vk_id, 
                    DENG_REGISTRY_TYPE_VK_TEXTURE, NULL);
            }


            // Set up image info structure for binding texture sampler
            VkDescriptorImageInfo desc_imageinfo{};
            desc_imageinfo.sampler = p_reg_vk_tex->vk_tex.sampler;
            desc_imageinfo.imageView = p_reg_vk_tex->vk_tex.image_view;
            desc_imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // Set the layout data for each swapchain image
            for(size_t i = 0; i < set_layouts.size(); i++)
                set_layouts[i] = __vk_DescriptorSetLayoutCreator::getLayout(assetModeToPipelineType(reg_asset.asset.asset_mode));

            // Allocate memory for descriptor set instances
            asset.desc_c = __max_frame_c;
            asset.desc_sets = (VkDescriptorSet*) calloc(asset.desc_c,
                sizeof(VkDescriptorSet));

            VkDescriptorSetAllocateInfo allocinfo = {};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocinfo.descriptorPool = __vk_DescriptorPoolCreator::getDescPool(assetModeToPipelineType(reg_asset.asset.asset_mode));
            allocinfo.descriptorSetCount = static_cast<deng_ui32_t>(set_layouts.size());
            allocinfo.pSetLayouts = set_layouts.data();
            
            // Allocate descriptor sets
            if(vkAllocateDescriptorSets(device, &allocinfo, asset.desc_sets) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets");

            // Iterate through every descriptor set in swapchain image and update it
            for(size_t i = 0; i < __max_frame_c; i++) {
                // Find all required buffer infos
                std::vector<VkDescriptorBufferInfo> buffer_infos = __findBufferInfos(reg_asset.asset,
                    bd, i, __max_frame_c, min_align, ubo_chunk_size);


                // Find write descriptors for texture mapped asset
                std::vector<VkWriteDescriptorSet> write_descs = __mkMappedWriteDescInfos(reg_asset.asset.asset_mode,
                    buffer_infos, asset.desc_sets[i], desc_imageinfo);

                // Update texture mapped descriptor sets
                vkUpdateDescriptorSets(device, static_cast<deng_ui32_t>(write_descs.size()), 
                    write_descs.data(), 0, NULL);
            }
        }

        
        /// Create descriptor sets for ImGui elements
        void __vk_DescriptorSetsCreator::mkUIDS ( 
            VkDevice device, 
            __vk_BufferData &bd, 
            deng_Id texture_atlas
        ) {
            // Retrieve the ui texture atlas' Vulkan instance
            RegData reg_tex = m_reg.retrieve(texture_atlas, DENG_REGISTRY_TYPE_TEXTURE, NULL);
            RegData reg_vk_tex = m_reg.retrieve(reg_tex.tex.vk_id, DENG_REGISTRY_TYPE_VK_TEXTURE, NULL);

            // Set up image info structure for binding texture sampler
            VkDescriptorImageInfo desc_imageinfo = {};
            desc_imageinfo.sampler = reg_vk_tex.vk_tex.sampler;
            desc_imageinfo.imageView = reg_vk_tex.vk_tex.image_view;
            desc_imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            // Set the layout data for each swapchain image
            std::vector<VkDescriptorSetLayout> set_layouts(__max_frame_c);
            for(size_t i = 0; i < set_layouts.size(); i++)
                set_layouts[i] = __vk_DescriptorSetLayoutCreator::getLayout(DENG_PIPELINE_TYPE_UI);

            // Resize the desc set vector to have an capacity to store descriptor sets for each frame in flight
            m_ui_desc_sets.resize(__max_frame_c);

            VkDescriptorSetAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocinfo.descriptorPool = __vk_DescriptorPoolCreator::getDescPool(DENG_PIPELINE_TYPE_UI);
            allocinfo.descriptorSetCount = static_cast<deng_ui32_t>(set_layouts.size());
            allocinfo.pSetLayouts = set_layouts.data();
            
            // Allocate descriptor sets
            if(vkAllocateDescriptorSets(device, &allocinfo, m_ui_desc_sets.data()) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets");

            // Iterate through every descriptor set in swapchain image and update it
            for(size_t i = 0; i < __max_frame_c; i++) {
                // Find write descriptors for texture mapped asset
                const VkWriteDescriptorSet write_desc_set = __mkUIWriteDescInfo(m_ui_desc_sets[i], desc_imageinfo);

                // Update texture mapped descriptor sets
                vkUpdateDescriptorSets(device, 1, &write_desc_set, 0, NULL);
            }
        }


        // Getter methods
        const std::vector<VkDescriptorSet> &__vk_DescriptorSetsCreator::getUIDS() {
            return m_ui_desc_sets;
        }
    }   
}
