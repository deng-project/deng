/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: unmapped.vert - GLSL vertex shader for 3D unmapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable

// Uniform data structure
layout(binding = 0) uniform UniformData {
    mat4 transform;
    mat4 view;
} ubo;


// Shader input data
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm_pos;

void main() {
    gl_Position = vec4(in_pos, 1.0f);
}
