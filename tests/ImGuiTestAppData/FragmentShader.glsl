#version 450
#extension GL_ARB_separate_shader_objects : enable

// input
layout(binding = 1) uniform sampler2D tex_sampler; 
layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 col_mul;

// output
layout(location = 0) out vec4 color;

void main() {
    color = texture(tex_sampler, uv) * col_mul;
}
