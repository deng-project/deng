/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: tex_mapped.frag - GLSL vertex shader for 3D texture mapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable


const int max_light_src_c = 8;


struct Light {
    vec4 pos;
    float intensity;
    uint pad1;
    vec2 pad2;
};

/// Uniform object for containing data about the asset
layout(std140, binding = 1) uniform AssetData {
    vec4 kd;
    vec4 ka;
    vec4 ks;
    float phong_exp;
    uint ignore_transform;
    uint is_unmapped;

    // Padding
    uint pad;
} asset_data;


///// Uniform buffer of all lighting objects to use
layout(std140, binding = 2) uniform LightData {
    // Light source data
    Light light[max_light_src_c];
    uint light_src_c;

    // Padding
    vec2 pad1;
    uint pad2;
} ld;

layout(binding = 3) uniform sampler2D tex_sampler;


// Fragment input data
layout(location = 0) in vec2 in_tex;
layout(location = 1) in vec3 in_vert;
layout(location = 2) in vec4 in_pos;
layout(location = 3) in vec3 in_normal;

// Output fragment
layout(location = 0) out vec4 out_color;

void main() {
    //vec4 dif_color = asset_data.kd;

    //// Check if color or texture mapping should be used
    //if(asset_data.is_unmapped == 1)
        //dif_color = asset_data.kd;
    //else dif_color = texture(tex_sampler, in_tex);

    //out_color = asset_data.ka * dif_color;

    //// For each light source apply Blinn-Phong shading
    //for(uint i = 0; i < ld.light_src_c; i++) {
        //vec3 light_ray = normalize(vec3(ld.light[i].pos) - in_vert);

        //// Cosine of the angle between surface to light position vector and 
        //// vertex normal vector
        //float cos_theta = dot(light_ray, normalize(in_normal));
        
        //// Calculate the half vector between surface to light source and 
        //// surface to view vectors
        //vec3 hf = normalize(vec3(in_pos) + light_ray);

        //// Calculate the cosine of the angle between vertex normal and 
        //// half vector
        //float cos_alpha = dot(normalize(in_normal), hf);

        //out_color += dif_color * ld.light[i].intensity * clamp(cos_theta, 0, 1) +
            //asset_data.ks * ld.light[i].intensity * pow(clamp(cos_alpha, 0, 1), asset_data.phong_exp);
    //}

    out_color = texture(tex_sampler, in_tex);
    //out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
