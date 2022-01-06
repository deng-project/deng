// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: tex_mapped.vert - GLSL vertex shader for 3D texture mapped assets
// author: Karl-Mihkel Ott

#version 450
#extension GL_ARB_separate_shader_objects : enable

// Uniform trasformation structure
layout(std140, binding = 0) uniform UniformData {
    mat4 transform;
    mat4 view;
} ubo;


// Input data
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_tex_pos;
layout(location = 2) in vec3 in_norm_pos;


void main() {
    // Check how the position should be transformed
    gl_Position = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0f);
}
