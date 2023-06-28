#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vInputPosition;
layout(location = 1) in vec2 vInputUV;
layout(location = 0) out vec4 vOutputFrag;
layout(set = 0, binding = 1) uniform sampler2D smpTerrain;

void main() {
	vOutputFrag = texture(smpTerrain, vInputUV);
}