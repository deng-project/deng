#version 450
#extension GL_ARB_separate_shader_objects : enable

// input data
layout(location = 0) in vec2 i_uv;

layout(binding = 3) uniform sampler2D tex_sampler;

// output data
layout(location = 0) out vec4 o_color;

void main() {
    o_color = texture(tex_sampler, i_uv);
}
