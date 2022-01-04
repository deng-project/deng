/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: light_man.cpp - light manager class implementation
/// author: Karl-Mihkel Ott


#define __LIGHT_MANAGER_CPP
#include <deng/lighting/light_man.h>


namespace deng {
        
    LightManager::LightManager(Registry &reg) : m_reg(reg) {
        // Reserve some memory for light sources in order to avoid reallocations
        m_pt_srcs.reserve(__DENG_MAX_LIGHT_SRC_COUNT);
        m_sun_srcs.reserve(__DENG_MAX_LIGHT_SRC_COUNT); 
        m_dir_srcs.reserve(__DENG_MAX_LIGHT_SRC_COUNT);
    }

    
    /// Create a new point light source
    deng_Id LightManager::newPtLightSrc (
        deng_vec_t intensity, 
        const dengMath::vec3<deng_vec_t> &pos
    ) {
        // Construct a new registry entry
        RegData lsrc = {};
        lsrc.pt_light.uuid = uuid_Generate();
        lsrc.pt_light.intensity = intensity;
        lsrc.pt_light.pos = pos;
        
        // Push the registry entry to registry
        m_reg.push(lsrc.pt_light.uuid, DENG_REGISTRY_TYPE_PT_LIGHT,
            lsrc);

        m_src_c++;
        m_pt_srcs.push_back(lsrc.pt_light.uuid);

        return lsrc.pt_light.uuid;
    }

    /// Create new sun light source that has collinear light rays
    deng_Id LightManager::newSunLightSrc (
        deng_vec_t sky_height, 
        deng_vec_t angle, 
        deng_vec_t intensity
    ) {
        // Construct a new registry entry
        RegData lsrc = {};
        lsrc.sun_light.uuid = uuid_Generate();
        lsrc.sun_light.sky_height = sky_height;
        lsrc.sun_light.angle = angle;
        lsrc.sun_light.intensity = intensity;

        // Push the registry entry to registry
        m_reg.push(lsrc.sun_light.uuid, DENG_REGISTRY_TYPE_SUN_LIGHT,
            lsrc);

        m_src_c++;
        m_pt_srcs.push_back(lsrc.sun_light.uuid);

        return lsrc.sun_light.uuid;
    }

    
    /// Create a new pointed directional light source
    deng_Id LightManager::newDirectionalLightSrc (
        deng_vec_t intensity, 
        deng_vec_t radius,
        const dengMath::vec3<deng_vec_t> &normal,
        const dengMath::vec3<deng_vec_t> &pos
    ) {
        // Construct a new registry entry
        RegData lsrc = {};
        lsrc.dir_light.uuid = uuid_Generate();
        lsrc.dir_light.intensity = intensity;
        lsrc.dir_light.radius = radius;
        lsrc.dir_light.normal = normal;
        lsrc.dir_light.pos = pos;

        // Push the registry entry to registry
        m_reg.push(lsrc.dir_light.uuid, DENG_REGISTRY_TYPE_DIR_LIGHT,
            lsrc);

        m_src_c++;
        m_pt_srcs.push_back(lsrc.dir_light.uuid);

        return lsrc.dir_light.uuid;
    }


    /// Move the light source in world coordinate space
    /// If the specified light source type is sun light then change sky height only
    void LightManager::movSrc(deng_Id id, const dengMath::vec3<deng_vec_t> &delta) {
        // Retrieve the light source with its type
        deng_RegistryType type = {};
        RegData &reg_src = m_reg.retrieve(id, DENG_REGISTRY_TYPE_PT_LIGHT |
            DENG_REGISTRY_TYPE_SUN_LIGHT | DENG_REGISTRY_TYPE_DIR_LIGHT,
            &type);

        // Check the recieved type
        switch(type) {
        case DENG_REGISTRY_TYPE_PT_LIGHT:
            reg_src.pt_light.pos += delta;
            break;

        case DENG_REGISTRY_TYPE_SUN_LIGHT:
            reg_src.sun_light.sky_height += delta.second;
            break;

        case DENG_REGISTRY_TYPE_DIR_LIGHT:
            reg_src.pt_light.pos += delta;
            break;

        default:
            break;
        }
    }

        
    /// Rotate directional light direction vector around its own starting point
    /// If the light source is something other than directional light, this method
    /// throws a runtime error
    void LightManager::dirLightSrcRotate(deng_Id id, const dengMath::vec2<deng_vec_t> &rot) {
        // Retrieve light source from registry
        RegData &reg_src = m_reg.retrieve(id, 
            DENG_REGISTRY_TYPE_DIR_LIGHT, NULL);

        // Create u axis rotation matrices
        dengMath::mat3<deng_vec_t> rot_u = dengMath::mat3<deng_vec_t> {
            dengMath::vec3<deng_vec_t>(1.0f, 0.0f, 0.0f),
            dengMath::vec3<deng_vec_t>{0.0f, cosf(rot.first), -sinf(rot.first)},
            dengMath::vec3<deng_vec_t>{0.0f, sinf(rot.first), cosf(rot.first)}
        };

        // Create v axis rotation matrices
        dengMath::mat3<deng_vec_t> rot_v = dengMath::mat3<deng_vec_t> {
            dengMath::vec3<deng_vec_t>{cosf(rot.second), 0.0f, -sinf(rot.second)},
            dengMath::vec3<deng_vec_t>(0.0f, 1.0f, 0.0f),
            dengMath::vec3<deng_vec_t>{sinf(rot.second), 0.0f, cosf(rot.second)}
        };

        reg_src.dir_light.normal = rot_u * reg_src.dir_light.normal;
        reg_src.dir_light.normal = rot_v * reg_src.dir_light.normal;
    }


    /// Getter methods
    deng_ui32_t LightManager::getMaxLightSrcCount() { return __DENG_MAX_LIGHT_SRC_COUNT; }


    /// Merge all light sources into one container
    std::vector<deng_Id> LightManager::getUniversalLightSources() {
        std::vector<deng_Id> light_srcs; 
        light_srcs.reserve(__DENG_MAX_LIGHT_SRC_COUNT);

        // Check if there are too many light sources and throw error if there is
        if(m_src_c > __DENG_MAX_LIGHT_SRC_COUNT) {
            RUN_ERR("LightManager::getMaxLightSrcCount()",
                "Too many light sources in one LightManager instance");
        }

        // Insert all light sources to the end of light vector
        light_srcs.insert(light_srcs.end(), m_pt_srcs.begin(), m_pt_srcs.end());
        light_srcs.insert(light_srcs.end(), m_sun_srcs.begin(), m_sun_srcs.end());
        light_srcs.insert(light_srcs.end(), m_dir_srcs.begin(), m_dir_srcs.end());

        return light_srcs;
    }
}
