#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1) uniform sampler2D sampTexture;
layout(location = 0) in vec2 vInputUV;
layout(location = 1) in vec4 vInputMultipliers;
layout(location = 0) out vec4 vOutputColor;

void main() {
	vOutputColor = vInputMultipliers * texture(sampTexture, vInputUV);
}