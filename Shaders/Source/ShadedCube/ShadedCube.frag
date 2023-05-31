#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;

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
	vec4 vColor;
};

layout(std430, set = 0, binding = 1) readonly buffer Lights {
	vec4 vAmbientColor;
	Light lights[];
} uboLights;

layout(std140, set = 1, binding = 2) uniform Material {
	vec4 vAmbient;
	vec4 vDiffuse;
	vec4 vSpecular;
	float fShininess;
} uboMaterial;

void main() {
	// ambient color
	const vec3 vAmbientColor = uboLights.vAmbientColor.xyz * uboMaterial.vAmbient.xyz;
	
	// diffuse and specular
	vec3 vDiffuse = vec3(0.f);
	vec3 vSpecular = vec3(0.f);
	
	for (int i = 0; i < int(uboLights.vAmbientColor.w); i++) {
		// diffuse
		vec3 vNormal = normalize(vInputNormal);
		vec3 vLightDir = normalize(uboLights.lights[i].vPosition.xyz - vInputPosition);
		
		float fDiffuseImpact = max(dot(vNormal, vLightDir), 0.f);
		vDiffuse += uboLights.lights[i].vColor.xyz * (fDiffuseImpact * uboMaterial.vDiffuse.xyz);
	
		// specular
		vec3 vViewDir = normalize(uboCamera.vPosition.xyz - vInputPosition);
		vec3 vReflectDir = reflect(-vLightDir, vNormal);
		
		float fSpecular = pow(max(dot(vViewDir, vReflectDir), 0.f), uboMaterial.fShininess * 128.f);
		vSpecular += uboLights.lights[i].vColor.xyz * (fSpecular * uboMaterial.vSpecular.xyz);
	}
	
	vFragColor = vec4(vDiffuse + vAmbientColor + vSpecular, 1.f);
}