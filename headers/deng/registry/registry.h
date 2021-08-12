/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: registry.cpp - asset registry class header
/// author: Karl-Mihkel Ott


#ifndef __REGISTRY_H
#define __REGISTRY_H


#ifdef __REGISTRY_CPP
    #include <vector>
    #include <utility>
    #include <stdlib.h>
    #include <cstring>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    
    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
#endif

#include <deng/lighting/light_srcs.h>
#include <deng/opengl/resources.h>
#include <deng/vulkan/assets.h>


namespace deng {

    /// Data union for all suitable registry types that can be submitted
    union RegData {
        das_Asset asset;
        das_Texture tex;
        vulkan::__vk_Asset vk_asset;
        vulkan::__vk_Texture vk_tex;
        opengl::__gl_Texture gl_tex;
        __PtLightSrc pt_light;
        __SunLightSrc sun_light;
        __DirectionLightSrc dir_light;
    };

    
    /// Main data structure for keeping information about hash mapped 
    /// registry entry
    struct __RegEntry {
        RegData element;
        deng_RegistryType type;
        deng_ui32_t index;
    };
    

    /// Global registry entries' handler class
    class Registry {
    private:
        // Each type has its element pointers stored in Registry class
        std::vector<deng_Id> m_assets;
        std::vector<deng_Id> m_textures;
        std::vector<deng_Id> m_vk_assets;
        std::vector<deng_Id> m_vk_textures;
        std::vector<deng_Id> m_gl_textures;
        std::vector<deng_Id> m_pt_lights;
        std::vector<deng_Id> m_sun_lights;
        std::vector<deng_Id> m_dir_lights;

        std::vector<__RegEntry> m_entries;
        Hashmap m_map;

    private:
        __RegEntry *__findElemPtr(deng_Id id, deng_RegistryTypeBitMask expected_type,
            deng_RegistryType *p_type_feedback);

    public:
        Registry();
        ~Registry();


        /// Push given data into registry and throw an exeption, when the element already exists
        void push(const deng_Id id, const deng_RegistryType type, const RegData &data);


        /// Retrieve and verify entry from registry
        /// An runtime error is thrown if the registry entry type does not correspond to
        /// the expected registry entry type or if the specified id is invalid
        RegData &retrieve(const deng_Id id, const deng_RegistryTypeBitMask expected_type_mask,
            deng_RegistryType *p_type_feedback);


        /// Retrieve and verify the pointer of an entry from registry
        /// An runtime error is thrown if the registry entry type does not correspond to
        /// the expected registry entry type or if id is invalid
        RegData *retrievePtr(deng_Id id, deng_RegistryTypeBitMask expected_type_mask,
            deng_RegistryType *p_type_feedback);

        
        /// Pop an entry from from registry 
        /// An runtime error is thrown if the registry entry type does not correspond to
        /// the expected registry entry type or if the specified id is invalid
        RegData pop(deng_Id id, deng_RegistryTypeBitMask expected);


        /// Find the size of total registry elements
        size_t size() const;
        

        /// Find all elements in the registry
        const std::vector<RegData*> all();


        /// Get all registry entries that have common type
        const std::vector<RegData*> getItemsByType(const deng_RegistryType type);
    };
}

#endif
