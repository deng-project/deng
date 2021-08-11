/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: registry.cpp - asset registry class implementation
/// author: Karl-Mihkel Ott

#define __REGISTRY_CPP
#include <deng/registry/registry.h>


namespace deng {

    __GlobalRegistry::__GlobalRegistry() {
        newHashmap(&m_map, 32);
        m_entries.reserve(32);
    }


    /// Basically perform element cleanup when possible
    __GlobalRegistry::~__GlobalRegistry() {
        // Free all memory allocated for assets or textures
        for(size_t i = 0; i < m_entries.size(); i++) {
            switch(m_entries[i].type) {
            case DENG_SUPPORTED_REG_TYPE_ASSET:
                // Initially clean the position indices and uuid
                free(m_entries[i].element.asset.uuid);
                if(m_entries[i].element.asset.is_opengl)
                    free(m_entries[i].element.asset.indices.gl);

                // Check which cleaup to perform
                switch(m_entries[i].element.asset.asset_mode) {
                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                    if(m_entries[i].element.asset.is_opengl)
                        free(m_entries[i].element.asset.vertices.v2d.mer.vert);
                    else {
                        free(m_entries[i].element.asset.vertices.v2d.mul.pos);
                        free(m_entries[i].element.asset.indices.pos);

                        free(m_entries[i].element.asset.vertices.v2d.mul.tex);
                        free(m_entries[i].element.asset.indices.tex);
                    }
                    break;

                case DAS_ASSET_MODE_2D_UNMAPPED:
                    if(m_entries[i].element.asset.is_opengl)
                        free(m_entries[i].element.asset.vertices.v2d.mer.uvert);
                    else {
                        free(m_entries[i].element.asset.vertices.v2d.mul.pos);
                        free(m_entries[i].element.asset.indices.pos);
                    }
                    break;

                case DAS_ASSET_MODE_3D_UNMAPPED:
                    if(m_entries[i].element.asset.is_opengl)
                        free(m_entries[i].element.asset.vertices.v3d.mer.uvert);
                    else {
                        free(m_entries[i].element.asset.vertices.v3d.mul.pos);
                        free(m_entries[i].element.asset.indices.pos);

                        // Clean vertex normals and their indices
                        free(m_entries[i].element.asset.indices.norm);
                    }
                    break;

                case __DAS_ASSET_MODE_3D_UNMAPPED_UNOR:
                    // Irregular asset mode, assume multiple index buffers
                    free(m_entries[i].element.asset.vertices.v3d.mul.pos);
                    free(m_entries[i].element.asset.indices.pos);
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    if(m_entries[i].element.asset.is_opengl)
                        free(m_entries[i].element.asset.vertices.v3d.mer.vert);
                    else {
                        free(m_entries[i].element.asset.vertices.v3d.mul.pos);
                        free(m_entries[i].element.asset.indices.pos);

                        free(m_entries[i].element.asset.vertices.v3d.mul.tex);
                        free(m_entries[i].element.asset.indices.tex);

                        free(m_entries[i].element.asset.vertices.v3d.mul.norm);
                        free(m_entries[i].element.asset.indices.norm);
                    }
                    break;

                case __DAS_ASSET_MODE_3D_TEXTURE_MAPPED_UNOR:
                    // Irregular asset mode, assume multiple index buffers
                    free(m_entries[i].element.asset.vertices.v3d.mul.pos);
                    free(m_entries[i].element.asset.indices.pos);

                    free(m_entries[i].element.asset.vertices.v3d.mul.tex);
                    free(m_entries[i].element.asset.indices.tex);
                    break;

                default:
                    break;
                }
                break;

            case DENG_SUPPORTED_REG_TYPE_VK_ASSET:
                free(m_entries[i].element.vk_asset.uuid);
                free(m_entries[i].element.vk_asset.desc_sets);
                break;

            case DENG_SUPPORTED_REG_TYPE_TEXTURE:
                if(!m_entries[i].element.tex.no_reg_cleanup) {
                    free(m_entries[i].element.tex.pixel_data.pixel_data);
                    free(m_entries[i].element.tex.uuid);
                }
                break;

            case DENG_SUPPORTED_REG_TYPE_VK_TEXTURE:
                free(m_entries[i].element.vk_tex.uuid);
                break;

            case DENG_SUPPORTED_REG_TYPE_PT_LIGHT:
                free(m_entries[i].element.pt_light.uuid);
                break;
                
            case DENG_SUPPORTED_REG_TYPE_DIR_LIGHT:
                free(m_entries[i].element.dir_light.uuid);
                break;

            case DENG_SUPPORTED_REG_TYPE_SUN_LIGHT:
                free(m_entries[i].element.sun_light.uuid);
                break;

            default:
                break;
            }
        }
        destroyHashmap(&m_map);
    }


    __RegEntry *__GlobalRegistry::__findElemPtr (
        deng_Id id,
        deng_SupportedRegTypeBitMask expected_type,
        deng_SupportedRegType *p_type_feedback
    ) {
        // Find the pointer value from the map
        __RegEntry *p_entry = (__RegEntry*) findValue(&m_map,
            (void*) id, strlen(id));

        if(!p_entry)
            RUN_ERR("__GlobalRegistry::retrieve()", "Invalid element id \"" + std::string(id) + "\"");

        // Entry type check
        if((p_entry->type & expected_type) != p_entry->type) {
            RUN_ERR("__GlobalRegistry::retrieve()", 
                "Expected and retrieved registry entry types do not match");
        }

        // Check if the feedback value should be set
        if(p_type_feedback)
            *p_type_feedback = p_entry->type;

        return p_entry;
    }

    
    /// Register given data for usage
    /// A runtime error is thrown if the registry already has an element with the same id as specified one
    void __GlobalRegistry::push (
        deng_Id uuid, 
        deng_SupportedRegType type, 
        const RegType &data
    ) {
        void *val = findValue(&m_map, uuid,
            strlen(uuid));

        if(val)
            RUN_ERR("__GlobalRegistry::push()", "Value with identifier \"" + std::string(uuid) + "\" already exists");
    
        // Push the element to entries and 
        std::vector<vulkan::__vk_Asset> asset;
        m_entries.push_back(__RegEntry{data, type, (deng_ui32_t) m_entries.size()});
        pushToHashmap(&m_map, uuid, strlen(uuid),
            &m_entries.back());
    }


    /// Retrieve and verify entry from registry
    /// An runtime error is thrown if the registry entry type does not correspond to
    /// the expected registry entry type or if id is invalid
    RegType &__GlobalRegistry::retrieve (
        deng_Id id,
        deng_SupportedRegTypeBitMask expected_type_mask,
        deng_SupportedRegType *p_type_feedback
    ) { 
        // Check if feedback type variable should be set
        return __findElemPtr(id, expected_type_mask, p_type_feedback)->element; 
    }


    /// Retrieve and verify the pointer of an entry from registry
    /// An runtime error is thrown if the registry entry type does not correspond to
    /// the expected registry entry type or if id is invalid
    RegType *__GlobalRegistry::retrievePtr (
        deng_Id id,
        deng_SupportedRegTypeBitMask expected_type_mask,
        deng_SupportedRegType *p_type_feedback
    ) { return &__findElemPtr(id, expected_type_mask, p_type_feedback)->element; } 


    /// Pop an entry from from registry
    /// An runtime error is thrown if the id is invalid
    RegType __GlobalRegistry::pop(deng_Id id) {
        __RegEntry *p_entry = (__RegEntry*) popFromHashmap (
            &m_map,
            id,
            strlen(id)
        );

        if(!p_entry)
            RUN_ERR("__GlobalRegistry::pop()", "Invalid entry ID");

        return p_entry->element;
    }


    /// Find the size of total registry elements
    size_t __GlobalRegistry::size() const {
        return m_entries.size();
    }
    

    /// Get all elements in the registry
    const std::vector<__RegEntry> &__GlobalRegistry::all() {
        return m_entries;
    }
}
