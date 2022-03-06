#version 450
#extension GL_ARB_separate_shader_objects : enable

// input
layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_mul;

// output
layout(location = 0) out vec2 o_uv;
layout(location = 1) out vec2 o_mul;

void main() {
    gl_Position = vec4(i_pos, 0.0f, 1.0f);
    o_uv = i_uv;
    i_mul = o_mul;
}
