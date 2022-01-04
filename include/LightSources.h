/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: LightSources.h - DENG light sources header file
/// author: Karl-Mihkel Ott


#ifndef LIGHT_SOURCES_H
#define LIGHT_SOURCES_H
#define __DENG_MAX_LIGHT_SRC_COUNT      8


namespace deng {

    /// Structure for point light source information keeping
    struct __PtLightSrc {
        deng_Id uuid;
        deng_vec_t intensity;
        dengMath::vec3<deng_vec_t> pos;
    };

    
    /// Structure for keeping information about global light source
    /// that has specific normalised ray vector position
    struct __SunLightSrc {
        deng_Id uuid;
        deng_vec_t sky_height;
        deng_vec_t angle;
        deng_vec_t intensity;
    };

    
    /// Structure for keeping information about direction light source
    struct __DirectionLightSrc {
        deng_Id uuid;
        deng_vec_t intensity;
        deng_vec_t radius;
        dengMath::vec3<deng_vec_t> normal;
        dengMath::vec3<deng_vec_t> pos;
    };


    /// Universal light source union
    union __UniLightSrc {
        __PtLightSrc pt;
        __SunLightSrc sun;
        __DirectionLightSrc dir;
    };

    
    /// Light source type specifier
    enum __LightSrcType {
        DENG_LIGHT_SRC_TYPE_NONE    = 0,
        DENG_LIGHT_SRC_TYPE_PT      = 1,
        DENG_LIGHT_SRC_TYPE_SUN     = 2,
        DENG_LIGHT_SRC_TYPE_DIR     = 3,
        DENG_LIGHT_SRC_TYPE_FIRST   = DENG_LIGHT_SRC_TYPE_NONE,
        DENG_LIGHT_SRC_TYPE_LAST    = DENG_LIGHT_SRC_TYPE_DIR
    };

    
    /// Main light source specifier structure
    struct LightSource {
        __LightSrcType type;
        __UniLightSrc light;
    };
}

#endif
