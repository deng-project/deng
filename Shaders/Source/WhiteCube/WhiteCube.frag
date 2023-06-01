#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;
layout(location = 2) in vec2 vInputUV;

layout(location = 0) out vec4 vFragColor;

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera; 

struct Light {
	vec4 vPosition;
	vec4 vDiffuse;
	vec4 vSpecular;
};

layout(std140, set = 0, binding = 1) readonly buffer Lights {
	uint uLightsCount;
	vec3 vAmbient;
	Light lights[];
} uboLights;

layout(std140, set = 1, binding = 2) uniform Material {
	vec4 vAmbient;
	vec4 vDiffuse;
	vec4 vSpecular;
	uvec4 vMaps;
	float fShininess;
} uboMaterial;

void main() {
	vFragColor = vec4(1.f, 1.f, 1.f, 1.f);
}