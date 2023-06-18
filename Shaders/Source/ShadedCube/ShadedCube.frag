#version 450
#extension GL_ARB_separate_shader_objects : enable

#define SHINE_MUL 128.f

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;
layout(location = 2) in vec2 vInputUV;
layout(location = 3) in flat int iInstanceIndex;

layout(location = 0) out vec4 vFragColor;

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera;

struct PointLight {
	vec4 vPosition;
	vec4 vDiffuse;
	vec4 vSpecular;
	vec3 vK;
};

struct DirectionalLight {
	vec4 vDirection;
	vec4 vDiffuse;
	vec4 vSpecular;
};


struct SpotLight {
	vec4 vPosition;
	vec4 vDirection;
	vec4 vDiffuse;
	vec4 vSpecular;
	float fInnerCutoff;
	float fOuterCutoff;
};

struct DrawDescriptorIndices {
	ivec4 indices;
};

layout(std430, set = 0, binding = 0) readonly buffer DrawDescriptorIndicesSSBO {
	DrawDescriptorIndices descriptors[];
} uboIndices;


layout(std430, set = 0, binding = 2) readonly buffer Lights1 {
	vec4 vAmbient;
	// x: point light count; y: directional light count; z: spot light count
	uvec4 vLightCounts;
	PointLight pointLights[];
} uboPointLights;


layout(std430, set = 0, binding = 3) readonly buffer Lights2 {
	DirectionalLight dirLights[];
} uboDirLights;


layout(std430, set = 0, binding = 4) readonly buffer Lights3 {
	SpotLight spotLights[];
} uboSpotLights;


struct Material {
	vec4 vAmbient;
	vec4 vDiffuse;
	vec4 vSpecular;
	uvec4 vMaps;
	float fShininess;
};

layout(std140, set = 0, binding = 5) readonly buffer MaterialSSBO {
	Material materials[];
} uboMaterial;

layout(set = 1, binding = 0) uniform sampler2D smpDiffuse;
layout(set = 1, binding = 1) uniform sampler2D smpSpecular;


vec3 g_vDiffuse = vec3(0.f);
vec3 g_vSpecular = vec3(0.f);

vec3 calculateDiffuse(vec3 vLightDir, vec3 vLightDiffuse) {
	const int ciIndex = uboIndices.descriptors[iInstanceIndex].indices.y;
	
	const vec3 vNormal = normalize(vInputNormal);
	const float fDiffuseImpact = max(dot(vNormal, vLightDir), 0.f);
	
	// material diffuse
	if (uboMaterial.materials[ciIndex].vMaps[0] == 0) {
		return vLightDiffuse * (fDiffuseImpact * uboMaterial.materials[ciIndex].vDiffuse.xyz);
	}
	// diffuse map is used
	else {
		return vLightDiffuse * (fDiffuseImpact * vec3(texture(smpDiffuse, vInputUV)));
	}
}

vec3 calculateSpecular(vec3 vLightDir, vec3 vLightSpecular) {
	const int ciIndex = uboIndices.descriptors[iInstanceIndex].indices.y;
	
	const vec3 vNormal = normalize(vInputNormal);
	const vec3 vViewDir = normalize(uboCamera.vPosition.xyz - vInputPosition);
	const vec3 vReflectDir = reflect(-vLightDir, vNormal);
	const float fSpecularImpact = pow(max(dot(vViewDir, vReflectDir), 0.f), uboMaterial.materials[ciIndex].fShininess * SHINE_MUL);
	
	// material specular
	if (uboMaterial.materials[ciIndex].vMaps[2] == 0) {
		return vLightSpecular * (fSpecularImpact * uboMaterial.materials[ciIndex].vSpecular.xyz);
	}
	// specular map
	else {
		return vLightSpecular * (fSpecularImpact * vec3(texture(smpSpecular, vInputUV)));
	}
}

void calculatePointLights() {
	for (uint i = 0; i < uboPointLights.vLightCounts.x; i++) {
		vec3 vLightDir = uboPointLights.pointLights[i].vPosition.xyz - vInputPosition;
		const float fDistance = length(vLightDir);
		const float fAttenuation = 1.f / (uboPointLights.pointLights[i].vK[0] + 
			uboPointLights.pointLights[i].vK[1] * fDistance + uboPointLights.pointLights[i].vK[2] * (fDistance * fDistance));
		
		vLightDir = normalize(vLightDir);
		
		g_vDiffuse += fAttenuation * calculateDiffuse(vLightDir, uboPointLights.pointLights[i].vDiffuse.xyz);
		g_vSpecular += fAttenuation * calculateSpecular(vLightDir, uboPointLights.pointLights[i].vSpecular.xyz);
	}
}

void calculateDirLights() {
	// for each directional light
	for (uint i = 0; i < uboPointLights.vLightCounts.y; i++) {
		const vec3 vLightDir = normalize(-uboDirLights.dirLights[i].vDirection.xyz);
		
		g_vDiffuse += calculateDiffuse(vLightDir, uboDirLights.dirLights[i].vDiffuse.xyz);
		g_vSpecular += calculateDiffuse(vLightDir, uboDirLights.dirLights[i].vSpecular.xyz);
	}
}

void calculateSpotLights() {
	// for each spotlight
	for (uint i = 0; i < uboPointLights.vLightCounts.z; i++) {
		const vec3 vLightDir = normalize(uboSpotLights.spotLights[i].vPosition.xyz - vInputPosition);
		const float fTheta = dot(vLightDir, normalize(-uboSpotLights.spotLights[i].vDirection.xyz));
		const float fIntensity = clamp((fTheta - uboSpotLights.spotLights[i].fOuterCutoff) / (uboSpotLights.spotLights[i].fInnerCutoff - uboSpotLights.spotLights[i].fOuterCutoff), 0.f, 1.f);
		
		g_vDiffuse += fIntensity * calculateDiffuse(vLightDir, uboSpotLights.spotLights[i].vDiffuse.xyz);
		g_vSpecular += fIntensity * calculateSpecular(vLightDir, uboSpotLights.spotLights[i].vSpecular.xyz);
	}
}

vec3 calculateAmbient() {
	vec3 vAmbient = vec3(0.f);
	const int ciIndex = uboIndices.descriptors[iInstanceIndex].indices.y;
	
	if (uboMaterial.materials[ciIndex].vMaps[0] == 0) {
		vAmbient = uboPointLights.vAmbient.xyz * uboMaterial.materials[ciIndex].vAmbient.xyz;
	}
	else {
		vAmbient = uboPointLights.vAmbient.xyz * vec3(texture(smpDiffuse, vInputUV));
	}
	
	return vAmbient;
}


void main() {
	// ambient
	vec3 vAmbient = calculateAmbient();
	
	calculatePointLights();
	calculateDirLights();
	calculateSpotLights();
	
	vFragColor = vec4(vAmbient + g_vDiffuse + g_vSpecular, 1.f);
	//vFragColor = vec4(1.f, 0.f, 0.f, 1.f);
	
	// clamp fragment values to 1.0f
	if (vFragColor[0] > 1.f)
		vFragColor[0] = 1.f;
	if (vFragColor[1] > 1.f)
		vFragColor[1] = 1.f;
	if (vFragColor[2] > 1.f)
		vFragColor[2] = 1.f;
}