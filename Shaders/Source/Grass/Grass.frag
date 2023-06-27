#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vInputUV;
layout(location = 0) out vec4 vOutputColor;

layout(set = 0, binding = 0) uniform sampler2D smpTexture;

void main() {
	vOutputColor = texture(smpTexture, vInputUV);
	if (vOutputColor.a < 0.7) discard;
}