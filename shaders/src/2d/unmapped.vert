/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: unmapped.vert - GLSL vertex shader for 2D unmapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformData {
    mat3 view;
} ubo;


// Store color information about asset when it is not texture mapped
layout(binding = 1) uniform ColorData {
    vec4 color;
    uint is_transform;
    uint is_unmapped;

    // Padding bytes
    vec2 pad;
} cl;


layout(location = 0) in vec2 in_pos;
layout(location = 0) out vec4 out_color;

void main() {
    if(cl.is_transform == 0)
        gl_Position = vec4(in_pos, 0.0f, 1.0f);
    else {
        mat4 view_mat;
        view_mat[0] = vec4(ubo.view[0], 0.0f);
        view_mat[1] = vec4(ubo.view[1], 0.0f);
        view_mat[2] = vec4(ubo.view[2], 0.0f);
        view_mat[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        gl_Position = view_mat * vec4(in_pos, 0.0f, 1.0f);
    }
    
    // Fill the fragment shader data struct
    out_color = cl.color;
}
