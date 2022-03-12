#version 450
#extension GL_ARB_separate_shader_objects : enable

// input
layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec4 i_mul;
layout(binding = 0) uniform UniformBufferData { 
    vec2 size; 
} ubo;

// output
layout(location = 0) out vec2 o_uv;
layout(location = 1) out vec4 o_mul;

void main() {
    gl_Position = vec4((i_pos.x * 2 / ubo.size.x) - 1.0f, -(i_pos.y * 2 / ubo.size.y) + 1.0f, 0.0f, 1.0f);
    o_uv = i_uv;
    o_mul = i_mul;
}
