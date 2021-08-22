/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: tex_mapped.vert - GLSL vertex shader for 3D texture mapped assets
/// author: Karl-Mihkel Ott

#version 450
#extension GL_ARB_separate_shader_objects : enable

// Uniform trasformation structure
layout(std140, binding = 0) uniform UniformData {
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


// Input data
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex_pos;
layout(location = 2) in vec3 in_norm_pos;


// All the data that is passed to the fragment shader
layout(location = 0) out vec2 out_tex;
layout(location = 1) out vec3 out_vert;
layout(location = 2) out vec4 out_pos;
layout(location = 3) out vec3 out_normal;

void main() {
    // Check how the position should be transformed
    if(asset_data.ignore_transform == 1)
        gl_Position = ubo.view * vec4(in_pos.x, in_pos.y, in_pos.z, 1.0f);
    else gl_Position = ubo.transform * vec4(in_pos.x, in_pos.y, in_pos.z, 1.0f);

    /// DEBUGGING GHOST CODE:
    //if(ubo.transform[0][0] == 1)
        //gl_Position = vec4(in_pos, 1.0f);
    //else gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1.0f);

    // DO SOMETHING IN ORDER TO NOT DISCARD UniformData!!!
    //if(ubo.transform[0][0])
        //gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1.0f);
    //else gl_Position = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0f);

    // Set all the output variables
    out_tex = in_tex_pos;
    out_vert = vec3(in_pos.x, -in_pos.y, in_pos.z);
    out_pos = gl_Position;
    out_normal = vec3(in_norm_pos[0], -in_norm_pos[1], in_norm_pos[2]);
}
