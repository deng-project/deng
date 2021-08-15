/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: imgui.vert - GLSL vertex shader for ImGUI data rendering
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_col;

layout(location = 0) out vec2 out_tex;
layout(location = 1) out vec4 out_col_mul;

void main() {
    gl_Position = vec4(in_pos, 0.0f, 1.0f);
    out_tex = in_uv;
    out_col_mul = in_col;
}
