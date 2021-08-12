/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: unmapped.vert - GLSL vertex shader for 3D unmapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable

// Uniform data light source structure
// Currently only point light is supported
struct Light {
    float intensity;
    vec3 pos;
};

// Uniform data structure
layout(binding = 0) uniform UboTransform {
    mat4 transform;
    mat4 view;
} ubo;


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


// Shader input data
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm_pos;


// All the data that is passed to the fragment shader
layout(location = 0) out vec3 out_vert;
layout(location = 1) out vec4 out_pos;
layout(location = 2) out vec3 out_normal;

void main() {
    if(asset_data.ignore_transform == 1)
        gl_Position = ubo.view * vec4(in_pos[0], -in_pos[1], in_pos[2], 1.0f);
    else gl_Position = ubo.transform * vec4(in_pos[0], -in_pos[1], in_pos[2], 1.0f);

    // Set all the output variables
    out_vert = vec3(in_pos[0], -in_pos[1], in_pos[2]);
    out_pos = gl_Position;
    out_normal = vec3(in_norm_pos[0], -in_norm_pos[1], in_norm_pos[2]);
}
