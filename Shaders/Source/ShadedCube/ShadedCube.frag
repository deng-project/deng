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
	const vec3 vObjectColor = vec3(1.f, 1.f, 1.f);
	const vec3 vAmbientColor = vec3(0.1f, 0.1f, 0.1f);
	const float cfSpecularStrength = 0.5f;
	
	vec3 vDiffuse = vec3(0.f);
	vec3 vSpecular = vec3(0.f);
	
	for (int i = 0; i < 2; i++) {
		vec3 vNormal = normalize(vInputNormal);
		vec3 vLightDir = normalize(uboLights.lights[i].vPosition.xyz - vInputPosition);
		
		float fDiffuseImpact = max(dot(vNormal, vLightDir), 0.f);
		vDiffuse += fDiffuseImpact * uboLights.lights[i].vColor.xyz;
	
		vec3 vViewDir = normalize(uboCamera.vPosition.xyz - vInputPosition);
		vec3 vReflectDir = reflect(-vLightDir, vNormal);
		
		float fSpecular = pow(max(dot(vViewDir, vReflectDir), 0.f), 64);
		vSpecular = cfSpecularStrength * fSpecular * uboLights.lights[i].vColor.xyz;
	}
	vFragColor = vec4((vDiffuse + vAmbientColor + vSpecular) * vObjectColor, 1.f);
}