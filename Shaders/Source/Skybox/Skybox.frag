#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputUV;
layout(location = 0) out vec4 vOutputFrag;

layout(set = 0, binding = 1) uniform samplerCube smpSkybox;

void main() {
	vOutputFrag = texture(smpSkybox, vInputUV);
}