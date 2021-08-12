/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: registry.cpp - asset registry class implementation
/// author: Karl-Mihkel Ott

#define __REGISTRY_CPP
#include <deng/registry/registry.h>


namespace deng {

    Registry::Registry() {
        newHashmap(&m_map, 32);
        m_entries.reserve(32);
    }


    /// Basically perform element cleanup when possible
    Registry::~Registry() {
        // Free all memory allocated for assets or textures
        for(size_t i = 0; i < m_entries.size(); i++) {
            switch(m_entries[i].type) {
            case DENG_REGISTRY_TYPE_ASSET:
                // Initially clean the position indices and id
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

            case DENG_REGISTRY_TYPE_VK_ASSET:
                free(m_entries[i].element.vk_asset.uuid);
                free(m_entries[i].element.vk_asset.desc_sets);
                break;

            case DENG_REGISTRY_TYPE_TEXTURE:
                if(!m_entries[i].element.tex.no_reg_cleanup) {
                    free(m_entries[i].element.tex.pixel_data.pixel_data);
                    free(m_entries[i].element.tex.uuid);
                }
                break;

            case DENG_REGISTRY_TYPE_VK_TEXTURE:
                free(m_entries[i].element.vk_tex.uuid);
                break;

            case DENG_REGISTRY_TYPE_PT_LIGHT:
                free(m_entries[i].element.pt_light.uuid);
                break;
                
            case DENG_REGISTRY_TYPE_DIR_LIGHT:
                free(m_entries[i].element.dir_light.uuid);
                break;

            case DENG_REGISTRY_TYPE_SUN_LIGHT:
                free(m_entries[i].element.sun_light.uuid);
                break;

            default:
                break;
            }
        }
        destroyHashmap(&m_map);
    }


    __RegEntry *Registry::__findElemPtr (
        deng_Id id,
        deng_RegistryTypeBitMask expected_type,
        deng_RegistryType *p_type_feedback
    ) {
        // Find the pointer value from the map
        __RegEntry *p_entry = (__RegEntry*) findValue(&m_map,
            (void*) id, strlen(id));

        if(!p_entry)
            RUN_ERR("Registry::retrieve()", "Invalid element id \"" + std::string(id) + "\"");

        // Entry type check
        if((p_entry->type & expected_type) != p_entry->type) {
            RUN_ERR("Registry::retrieve()", 
                "Expected and retrieved registry entry types do not match");
        }

        // Check if the feedback value should be set
        if(p_type_feedback) *p_type_feedback = p_entry->type;

        return p_entry;
    }

    
    /// Register given data for usage
    /// A runtime error is thrown if the registry already has an element with the same id as specified one
    void Registry::push (
        deng_Id id, 
        deng_RegistryType type, 
        const RegData &data
    ) {
        void *val = findValue(&m_map, id,
            strlen(id));

        if(val)
            RUN_ERR("Registry::push()", "Value with identifier \"" + std::string(id) + "\" already exists");
    
        // Push the element to entries and 
        std::vector<vulkan::__vk_Asset> asset;
        m_entries.push_back(__RegEntry{data, type, (deng_ui32_t) m_entries.size()});
        pushToHashmap(&m_map, id, strlen(id),
            &m_entries.back());

        // Check the data type and push id to corresponding vector 
        switch(type) {
        case DENG_REGISTRY_TYPE_ASSET:
            m_assets.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_TEXTURE:
            m_textures.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_VK_ASSET:
            m_vk_assets.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_VK_TEXTURE:
            m_vk_textures.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_GL_TEXTURE:
            m_gl_textures.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_PT_LIGHT:
            m_pt_lights.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_SUN_LIGHT:
            m_sun_lights.push_back(id);
            break;

        case DENG_REGISTRY_TYPE_DIR_LIGHT:
            m_dir_lights.push_back(id);
            break;

        default: break;
        }
    }


    /// Retrieve and verify entry from registry
    /// An runtime error is thrown if the registry entry type does not correspond to
    /// the expected registry entry type or if id is invalid
    RegData &Registry::retrieve (
        deng_Id id,
        deng_RegistryTypeBitMask expected_type_mask,
        deng_RegistryType *p_type_feedback
    ) { 
        // Check if feedback type variable should be set
        return __findElemPtr(id, expected_type_mask, p_type_feedback)->element; 
    }


    /// Retrieve and verify the pointer of an entry from registry
    /// An runtime error is thrown if the registry entry type does not correspond to
    /// the expected registry entry type or if id is invalid
    RegData *Registry::retrievePtr (
        deng_Id id,
        deng_RegistryTypeBitMask expected_type_mask,
        deng_RegistryType *p_type_feedback
    ) { return &__findElemPtr(id, expected_type_mask, p_type_feedback)->element; } 


    /// Pop an entry from from registry
    /// An runtime error is thrown if the id is invalid
    RegData Registry::pop(deng_Id id, deng_RegistryTypeBitMask expected) {
        __RegEntry *p_entry = (__RegEntry*) popFromHashmap(&m_map, id, strlen(id)); 
        if(!p_entry) RUN_ERR("Registry::pop()", "Invalid entry ID");

        if(!(p_entry->type & expected)) {
            RUN_ERR("Registry::retrieve()",
                "Expected and retrieved registry entry types do not match");
        }


        return p_entry->element;
    }


    /// Find the size of total registry elements
    size_t Registry::size() const {
        return m_entries.size();
    }
    

    /// Get all elements in the registry
    const std::vector<RegData*> Registry::all() {
        std::vector<RegData*> out;
        for (size_t i = 0; i < m_entries.size(); i++)
            out[i] = &m_entries[i].element;

        return out;
    }


    const std::vector<RegData*> Registry::getItemsByType(const deng_RegistryType type) {
        std::vector<RegData*> out;
        std::vector<deng_Id> *type_ids;
        out.reserve(m_entries.size());

        switch (type) {
        case DENG_REGISTRY_TYPE_ASSET:
            type_ids = &m_assets;
            break;

        case DENG_REGISTRY_TYPE_TEXTURE:
            type_ids = &m_textures;
            break;

        case DENG_REGISTRY_TYPE_VK_ASSET:
            type_ids = &m_vk_assets;
            break;

        case DENG_REGISTRY_TYPE_VK_TEXTURE:
            type_ids = &m_vk_textures;
            break;

        case DENG_REGISTRY_TYPE_GL_TEXTURE:
            type_ids = &m_gl_textures;
            break;

        case DENG_REGISTRY_TYPE_PT_LIGHT:
            type_ids = &m_pt_lights;
            break;

        case DENG_REGISTRY_TYPE_SUN_LIGHT:
            type_ids = &m_sun_lights;
            break;

        case DENG_REGISTRY_TYPE_DIR_LIGHT:
            type_ids = &m_dir_lights;
            break;

        default: 
            RUN_ERR("Registry::getItemsByType()", "Invalid registry type");
        }


        // Iterate through each element and push the retrieved registry data to output vector 
        for (size_t i = 0; i < type_ids->size(); i++) {
            RegData *dat = retrievePtr(type_ids->at(i), type, NULL);
            out.push_back(dat);
        }

        return out;
    }
}
