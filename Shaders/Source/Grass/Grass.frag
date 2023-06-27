#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 vOutputColor;

layout(set = 0, binding = 0) uniform sampler2D smpTexture;

void main() {
	vOutputColor = vec4(0.192f, 0.608f, 0.329f, 1.f);
}