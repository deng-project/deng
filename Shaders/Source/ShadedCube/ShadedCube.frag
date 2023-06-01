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

layout(std430, set = 0, binding = 1) readonly buffer Lights {
	vec4 vAmbientColor;
	Light lights[];
} uboLights;

layout(std140, set = 1, binding = 2) uniform Material {
	vec4 vAmbient;
	vec4 vDiffuse;
	vec4 vSpecular;
	uvec4 vMaps;
	float fShininess;
} uboMaterial;

layout(set = 1, binding = 3) uniform sampler2D smpDiffuse;
layout(set = 1, binding = 4) uniform sampler2D smpSpecular;

void main() {
	// ambient
	vec3 vAmbient = vec3(0.f);
	
	if (uboMaterial.vMaps[0] == 0) {
		vAmbient = uboLights.vAmbientColor.xyz * uboMaterial.vAmbient.xyz;
	}
	else {
		vAmbient = uboLights.vAmbientColor.xyz * vec3(texture(smpDiffuse, vInputUV));
	}
	
	// diffuse and specular
	vec3 vDiffuse = vec3(0.f);
	vec3 vSpecular = vec3(0.f);
	
	for (int i = 0; i < int(uboLights.vAmbientColor.w); i++) {
		vec3 vNormal = normalize(vInputNormal);
		vec3 vLightDir = normalize(uboLights.lights[i].vPosition.xyz - vInputPosition);
		float fDiffuseImpact = max(dot(vNormal, vLightDir), 0.f);
			
		// diffuse
		if (uboMaterial.vMaps[0] == 0) {
			vDiffuse += uboLights.lights[i].vDiffuse.xyz * (fDiffuseImpact * uboMaterial.vDiffuse.xyz);
		}
		// diffuse map
		else {
			vDiffuse += uboLights.lights[i].vDiffuse.xyz * (fDiffuseImpact * vec3(texture(smpDiffuse, vInputUV)));
		}
	
		vec3 vViewDir = normalize(uboCamera.vPosition.xyz - vInputPosition);
		vec3 vReflectDir = reflect(-vLightDir, vNormal);
		float fSpecularImpact = pow(max(dot(vViewDir, vReflectDir), 0.f), uboMaterial.fShininess * 128.f);
		
		// specular
		if (uboMaterial.vMaps[1] == 0) {
			vSpecular += uboLights.lights[i].vSpecular.xyz * (fSpecularImpact * uboMaterial.vSpecular.xyz);
		}
		// specular map
		else {
			vSpecular += uboLights.lights[i].vSpecular.xyz * (fSpecularImpact * vec3(texture(smpSpecular, vInputUV)));
		}
	}
	
	vFragColor = vec4(vDiffuse + vAmbient + vSpecular, 1.f);
}