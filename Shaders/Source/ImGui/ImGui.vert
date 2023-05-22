#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 vInputPosition;
layout(location = 1) in vec2 vInputUV;
layout(location = 2) in vec4 vInputColorMultiplier;

layout (std140, set = 0, binding = 0) uniform UniformSize {
	vec2 vSize;
} uboSize;

layout (location = 0) out vec2 vOutputUV;
layout (location = 1) out vec4 vOutputMultiplier;

void main() {
	gl_Position = vec4(vInputPosition.x * 2 / uboSize.vSize.x - 1.0f, -vInputPosition.y * 2 / uboSize.vSize.y + 1.f, 0.f, 1.f);
	vOutputUV = vInputUV;
	vOutputMultiplier = vInputColorMultiplier;
}