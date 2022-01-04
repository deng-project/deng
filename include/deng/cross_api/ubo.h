/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubo.h - cross api uniform buffer object data structure definitions
/// author: Karl-Mihkel Ott


#ifndef __UBO
#define __UBO

namespace deng {

    /// NOTE: Generic uniform buffer data alignment is following
    /// fc * Transform + fc * Transform2D + fc * LightData + n * (fc * ColorData)
        
    /// Structure for passing uniform lighting data right to the shader
    struct __UniformLightSource {
        dengMath::vec4<deng_vec_t> pos;
        deng_vec_t intensity;

        // Padding
        deng_ui32_t pad[3];
    };


    /// Structure for containing information about 3D transformations
    struct __UniformObjectTransform {
        dengMath::mat4<deng_vec_t> transform;
        dengMath::mat4<deng_vec_t> view;
    };


    /// Structure for containing information about 2D transformations
    struct __UniformObjectTransform2D {
        dengMath::mat4<deng_vec_t> view;
    };


    /// Structure for storing material and transformation information 
    /// about the asset
    struct __UniformAssetData {
        dengMath::vec4<deng_vec_t> diffuse;
        dengMath::vec4<deng_vec_t> ambient;
        dengMath::vec4<deng_vec_t> specular;
        deng_vec_t phong_exp;
        deng_ui32_t ignore_transform;
        deng_ui32_t is_unmapped;
        
        // Padding
        deng_ui32_t pad;
    };


    /// Uniform color data for 2D assets
    struct __UniformAssetData2D {
        dengMath::vec4<deng_vec_t> color;
        deng_ui32_t ignore_transform;
        deng_ui32_t is_unmapped;

        // Padding
        dengMath::vec2<deng_vec_t> pad;
    };


    /// Light data uniform structure
    struct __UniformLightData {
        __UniformLightData() { memset(this, 0, sizeof(__UniformLightData)); }
        __UniformLightSource light_srcs[__DENG_MAX_LIGHT_SRC_COUNT];
        deng_ui32_t light_src_c;

        // Padding needed for glsl uniforms
        deng_ui32_t pad[3];
    };
}

#endif
