#version 450
#extension GL_ARB_separate_shader_objects : enable

// input data
layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec2 i_uv;

// output data
layout (location = 0) out vec2 o_uv;

void main() {
    gl_Position = vec4(i_pos, 1.0f);
    o_uv = i_uv;
}
