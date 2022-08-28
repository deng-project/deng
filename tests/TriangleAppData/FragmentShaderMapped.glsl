#version 450
#extension GL_ARB_separate_shader_objects : enable

// input
layout(location = 0) in vec2 tex_coord;
layout(binding = 0) uniform sampler2D tex_sampler;

// output
layout(location = 0) out vec4 color;

void main() {
    color = texture(tex_sampler, tex_coord);
    //color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
