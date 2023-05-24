#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;

layout(location = 0) out vec4 vFragColor;

layout(std140, set = 0, binding = 0) uniform Camera {
	mat4 mView;
	mat4 mProjection;
	vec4 vPosition;
} uboCamera; 

struct Light {
	vec4 vPosition;
	vec4 vColor;
};

layout(std140, set = 0, binding = 2) uniform Lights {
	Light lights[2];
} uboLights;

void main() {
	vFragColor = vec4(1.f, 1.f, 1.f, 1.f);
}