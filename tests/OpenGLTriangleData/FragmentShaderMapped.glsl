#version 450
#extension GL_ARB_separate_shader_objects : enable

// input
layout(location = 0) in vec2 tex_coord;
layout(binding = 2) uniform sampler2D tex_sampler;

// output
layout(location = 0) out vec4 color;

void main() {
	color = texture(tex_sampler, tex_coord);
	// color = vec4(0.8f, 0.2f, 0.1f, 1.0f);
}
