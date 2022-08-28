#version 450
#extension GL_ARB_separate_shaders : enable

// input
layout(location = 0) in vec2 tex;

// output
layout(location = 0) out vec4 frag; 

// uniform
layout(binding = 0) uniform sampler2D sampler;


void main() {
	frag = texture(sampler, tex);
}