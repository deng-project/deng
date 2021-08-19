/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: light_man.h - light manager class header
/// author: Karl-Mihkel Ott


#ifndef __LIGHT_MANAGER_H
#define __LIGHT_MANAGER_H

#ifdef __LIGHT_MANAGER_CPP
    #include <stdlib.h>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <array>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <common/uuid.h>
    #include <data/assets.h>
    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/vulkan/resources.h>
    #include <deng/registry/registry.h>
#endif

namespace deng {
    
    /// Main light handling class
    class LightManager {
    private:
        std::vector<deng_Id> m_pt_srcs;
        std::vector<deng_Id> m_sun_srcs;
        std::vector<deng_Id> m_dir_srcs;

        deng_ui32_t m_src_c = 0;
        Registry &m_reg;

    public:
        LightManager(Registry &reg);

        /// Create a new point light source
        deng_Id newPtLightSrc(deng_vec_t intensity, 
            const dengMath::vec3<deng_vec_t> &pos);

        /// Create new sun light source that has collinear light rays
        deng_Id newSunLightSrc(deng_vec_t sky_height, deng_vec_t angle, 
            deng_vec_t intensity);

        
        /// Create a new pointed directional light source
        deng_Id newDirectionalLightSrc(deng_vec_t intensity, deng_vec_t radius,
            const dengMath::vec3<deng_vec_t> &normal, 
            const dengMath::vec3<deng_vec_t> &pos);


        /// Move the light source in world coordinate space
        /// If the specified light source type is sun light then change sky height only
        void movSrc(deng_Id src, const dengMath::vec3<deng_vec_t> &delta);

        
        /// Rotate directional light direction
        /// If the light source is something other than directional light, 
        /// throw runtime error
        void dirLightSrcRotate(deng_Id id, const dengMath::vec2<deng_vec_t> &rot);

    /// Getter methods
    public:
        deng_ui32_t getMaxLightSrcCount();


        /// Merge all light sources into one container
        std::vector<deng_Id> getUniversalLightSources();
    };

}

#endif
