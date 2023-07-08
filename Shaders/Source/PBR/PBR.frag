#version 450
#extension GL_ARB_separate_shader_objects : enable


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
	vec4 vColor;
};

struct DirectionalLight {
	vec4 vDirection;
	vec4 vColor;
};


struct SpotLight {
	vec4 vPosition;
	vec4 vDirection;
	vec4 vColor;
	float fInnerCutoff;
	float fOuterCutoff;
};

struct DrawDescriptorIndices {
	int iTransformIndex;
	int iMaterialIndex;
	int padding1;
	int padding2;
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
	vec4 vAlbedo;
	vec4 vEmission;
	float fRoughness;
	float fMetallic;
};

layout(std140, set = 0, binding = 5) readonly buffer MaterialSSBO {
	Material materials[];
} uboMaterial;

// utility macros
#define SQ(x) ((x)*(x))
#define PI 3.141592653589793f

// global variables
int g_ciMaterialIndex = uboIndices.descriptors[iInstanceIndex].iMaterialIndex;
vec3 g_cvView = normalize(uboCamera.vPosition.xyz - vInputPosition);
vec3 g_cvNormal = normalize(vInputNormal);
vec3 g_cvAlbedo = uboMaterial.materials[g_ciMaterialIndex].vAlbedo.xyz;
vec3 g_F0 = mix(vec3(0.04), g_cvAlbedo, uboMaterial.materials[g_ciMaterialIndex].fMetallic);
float g_fAlpha = SQ(uboMaterial.materials[g_ciMaterialIndex].fRoughness);
float g_fK = g_fAlpha / 2.0f;


vec3 Lambert(in vec3 vColor) {
	return vColor / PI;
}

// GGX/Throwbridge-Reitz normal distribution function
float D(in vec3 vHalf) {
	const float fSqAlpha = SQ(g_fAlpha);
	const float fNum = fSqAlpha;
	
	const float fNHDot = max(dot(g_cvNormal, vHalf), 0.0);
	const float fDenom = max(PI * SQ(SQ(fNHDot) * (fSqAlpha - 1.0) + 1.0), 0.00001);
	
	return fNum / fDenom;
}

float G1(in vec3 X) {
	const float fNXDot = max(dot(g_cvNormal, X), 0.0);
	const float fNum = fNXDot;
	const float fDenom = max(fNXDot * (1 - g_fK) + g_fK, 0.00001);
	
	return fNum / fDenom;
}

// Schlick-GGX, combination of Smith and Schlick-Beckmann geometry shadowing functions
float G(in vec3 vLightDir) {
	return G1(vLightDir)*G1(g_cvView);
}

// fresnel function
vec3 F(in vec3 vHalf) {
	return g_F0 + (vec3(1.0) - g_F0) * pow(1 - max(dot(vHalf, g_cvView), 0.0), 5.0);
}

vec3 Cook_Torrence(in vec3 vLightDir, in vec3 vHalf) {
	vec3 vNum = D(vHalf) * G(vLightDir) * F(vHalf);
	float fDenom = max(4.0 * max(dot(g_cvView, g_cvNormal), 0.0) * max(dot(vLightDir, g_cvNormal), 0.0), 0.00001);
	return vNum / fDenom;
}

vec3 BRDF(in vec3 vLightDir, in vec3 vColor) {
	const vec3 H = normalize(vLightDir + g_cvView);
	const vec3 Kd = (vec3(1.0) - F(H)) * (1.0 - uboMaterial.materials[g_ciMaterialIndex].fMetallic);
	return Kd * Lambert(vColor) + Cook_Torrence(vLightDir, H);
}

vec3 CalculatePointLights() {
	vec3 vOutput = vec3(0.0f);
	
	// for each point light
	for (uint i = 0; i < uboPointLights.vLightCounts.x; i++) {
		const vec3 vLightDir = normalize(uboPointLights.pointLights[i].vPosition.xyz - vInputPosition);
		const float fDistance = length(uboPointLights.pointLights[i].vPosition.xyz - vInputPosition);
		const float fAttenuation = 1.f / SQ(fDistance);
		
		const vec3 Li = fAttenuation * uboPointLights.pointLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, g_cvNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, g_cvAlbedo);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec3 CalculateSpotLights() {
	vec3 vOutput = vec3(0.0);

	// for each spot light
	for (uint i = 0; i < uboPointLights.vLightCounts.z; i++) {
		const vec3 vLightDir = normalize(uboSpotLights.spotLights[i].vPosition.xyz - vInputPosition);
		const float fTheta = max(dot(vLightDir, normalize(-uboSpotLights.spotLights[i].vDirection.xyz)), 0.0);
		
		const float fNum = fTheta - uboSpotLights.spotLights[i].fOuterCutoff;
		const float fDenom = max(uboSpotLights.spotLights[i].fInnerCutoff - uboSpotLights.spotLights[i].fOuterCutoff, 0.00001);
		const float fIntensity = clamp(fNum / fDenom, 0.f, 1.f);
		
		const vec3 Li = fIntensity * uboSpotLights.spotLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, g_cvNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, uboMaterial.materials[g_ciMaterialIndex].vAlbedo.xyz);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec3 CalculateDirectionalLights() {
	vec3 vOutput = vec3(0.0);
	
	// for each directional light
	for (uint i = 0; i < uboPointLights.vLightCounts.y; i++) {
		const vec3 vLightDir = normalize(-uboDirLights.dirLights[i].vDirection.xyz);
		const vec3 Li = uboDirLights.dirLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, g_cvNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, uboMaterial.materials[g_ciMaterialIndex].vAlbedo.xyz);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec4 PBR() {
	vec3 vOutput = uboMaterial.materials[g_ciMaterialIndex].vEmission.xyz;
	float w = uboMaterial.materials[g_ciMaterialIndex].vAlbedo.w;
	
	vOutput.xyz += CalculatePointLights();
	vOutput.xyz += CalculateSpotLights();
	vOutput.xyz += CalculateDirectionalLights();

	vec3 vAmbient = uboPointLights.vAmbient.xyz * g_cvAlbedo;
	vOutput += vAmbient;
	
	vOutput = vOutput / (vOutput + vec3(1.0));
	vOutput = pow(vOutput, vec3(1.0/2.2));

	return vec4(vOutput, w);
}

void main() {
	vFragColor = PBR();
	//vFragColor = vec4(1.f, 1.f, 1.f, 1.f);
}