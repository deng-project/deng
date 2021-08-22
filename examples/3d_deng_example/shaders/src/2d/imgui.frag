/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: imgui.frag - GLSL fragment shader for ImGUI data rendering
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(binding = 0) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_col_mul;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = in_col_mul * texture(tex_sampler, in_uv);
}
