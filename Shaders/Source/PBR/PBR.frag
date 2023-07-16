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
} pcCamera;

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
};

layout(std430, set = 0, binding = 0) readonly buffer DrawDescriptorIndicesSSBO {
	DrawDescriptorIndices descriptors[];
} ssboIndices;


layout(std430, set = 0, binding = 2) readonly buffer Lights1 {
	vec4 vAmbient;
	// x: point light count; y: directional light count; z: spot light count
	uvec4 vLightCounts;
	PointLight pointLights[];
} ssboPointLights;


layout(std430, set = 0, binding = 3) readonly buffer Lights2 {
	DirectionalLight dirLights[];
} ssboDirLights;


layout(std430, set = 0, binding = 4) readonly buffer Lights3 {
	SpotLight spotLights[];
} ssboSpotLights;

// sampler bit definitions
const uint bAlbedoMap 			= (1 << 0);
const uint bEmissionMap 		= (1 << 1);
const uint bNormalMap 			= (1 << 2);
const uint bMetallicMap 		= (1 << 3);
const uint bRoughnessMap 		= (1 << 4);
const uint bAmbientOcclusionMap	= (1 << 5);

struct Material {
	vec4 vAlbedo;
	vec4 vEmission;
	float fRoughness;
	float fMetallic;
	float fAmbientOcclusion;
	uint uSamplerBits;
};

layout(std140, set = 0, binding = 5) readonly buffer MaterialSSBO {
	Material materials[];
} ssboMaterial;

layout(set = 1, binding = 0) uniform sampler2D smpAlbedo;
layout(set = 1, binding = 1) uniform sampler2D smpEmission;
layout(set = 1, binding = 2) uniform sampler2D smpNormal;
layout(set = 1, binding = 3) uniform sampler2D smpMetallic;
layout(set = 1, binding = 4) uniform sampler2D smpRoughness;
layout(set = 1, binding = 5) uniform sampler2D smpAmbientOcclusion;

// utility macros
#define SQ(x) ((x)*(x))
#define PI 3.141592653589793f

// globals
int iMaterialIndex = ssboIndices.descriptors[iInstanceIndex].iMaterialIndex;
vec3 vView = normalize(pcCamera.vPosition.xyz - vInputPosition);

// material properties
vec3 vAlbedo = ssboMaterial.materials[iMaterialIndex].vAlbedo.xyz;
vec3 vEmission = ssboMaterial.materials[iMaterialIndex].vEmission.xyz;
vec3 vNormal = normalize(vInputNormal);
float fMetallic = ssboMaterial.materials[iMaterialIndex].fMetallic;
float fRoughness = ssboMaterial.materials[iMaterialIndex].fRoughness;
float fAmbientOcclusion = ssboMaterial.materials[iMaterialIndex].fAmbientOcclusion;

// globals that are derived from material properties
vec3 vF0 = vec3(0.0);
float fAlpha = 0.0;
float fK = 0.0;

vec3 Lambert(in vec3 vColor) {
	return vColor / PI;
}

// GGX/Throwbridge-Reitz normal distribution function
float D(in vec3 vHalf) {
	const float fSqAlpha = SQ(fAlpha);
	const float fNum = fSqAlpha;
	
	const float fNHDot = max(dot(vNormal, vHalf), 0.0);
	const float fDenom = max(PI * SQ(SQ(fNHDot) * (fSqAlpha - 1.0) + 1.0), 0.00001);
	
	return fNum / fDenom;
}

float G1(in vec3 X) {
	const float fNXDot = max(dot(vNormal, X), 0.0);
	const float fNum = fNXDot;
	const float fDenom = max(fNXDot * (1 - fK) + fK, 0.00001);
	
	return fNum / fDenom;
}

// Schlick-GGX, combination of Smith and Schlick-Beckmann geometry shadowing functions
float G(in vec3 vLightDir) {
	return G1(vLightDir) * G1(vView);
}

// fresnel function
vec3 F(in vec3 vHalf) {
	return vF0 + (vec3(1.0) - vF0) * pow(1 - max(dot(vHalf, vView), 0.0), 5.0);
}

vec3 Cook_Torrence(in vec3 vLightDir, in vec3 vHalf) {
	vec3 vNum = D(vHalf) * G(vLightDir) * F(vHalf);
	float fDenom = max(4.0 * max(dot(vView, vNormal), 0.0) * max(dot(vLightDir, vNormal), 0.0), 0.00001);
	return vNum / fDenom;
}

vec3 BRDF(in vec3 vLightDir, in vec3 vColor) {
	const vec3 H = normalize(vLightDir + vView);
	const vec3 Kd = (vec3(1.0) - F(H)) * (1.0 - fMetallic);
	return Kd * Lambert(vColor) + Cook_Torrence(vLightDir, H);
}

vec3 CalculatePointLights() {
	vec3 vOutput = vec3(0.0f);
	
	// for each point light
	for (uint i = 0; i < ssboPointLights.vLightCounts.x; i++) {
		const vec3 vLightDir = normalize(ssboPointLights.pointLights[i].vPosition.xyz - vInputPosition);
		const float fDistance = length(ssboPointLights.pointLights[i].vPosition.xyz - vInputPosition);
		const float fAttenuation = 1.f / SQ(fDistance);
		
		const vec3 Li = fAttenuation * ssboPointLights.pointLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, vNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, vAlbedo);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec3 CalculateSpotLights() {
	vec3 vOutput = vec3(0.0);

	// for each spot light
	for (uint i = 0; i < ssboPointLights.vLightCounts.z; i++) {
		const vec3 vLightDir = normalize(ssboSpotLights.spotLights[i].vPosition.xyz - vInputPosition);
		const float fTheta = max(dot(vLightDir, normalize(-ssboSpotLights.spotLights[i].vDirection.xyz)), 0.0);
		
		const float fNum = fTheta - ssboSpotLights.spotLights[i].fOuterCutoff;
		const float fDenom = max(ssboSpotLights.spotLights[i].fInnerCutoff - ssboSpotLights.spotLights[i].fOuterCutoff, 0.00001);
		const float fIntensity = clamp(fNum / fDenom, 0.f, 1.f);
		
		const vec3 Li = fIntensity * ssboSpotLights.spotLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, vNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, vAlbedo);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec3 CalculateDirectionalLights() {
	vec3 vOutput = vec3(0.0);
	
	// for each directional light
	for (uint i = 0; i < ssboPointLights.vLightCounts.y; i++) {
		const vec3 vLightDir = normalize(-ssboDirLights.dirLights[i].vDirection.xyz);
		const vec3 Li = ssboDirLights.dirLights[i].vColor.xyz;
		const float fDot = max(dot(vLightDir, vNormal), 0.0);
		const vec3 Fr = BRDF(vLightDir, vAlbedo);
		
		vOutput += (Li * Fr * fDot);
	}
	
	return vOutput;
}

vec4 PBR() {
	vec3 vOutput = vEmission;
	float w = ssboMaterial.materials[iMaterialIndex].vAlbedo.w;
	
	vOutput.xyz += CalculatePointLights();
	vOutput.xyz += CalculateSpotLights();
	vOutput.xyz += CalculateDirectionalLights();

	vec3 vAmbient = ssboPointLights.vAmbient.xyz * vAlbedo * fAmbientOcclusion;
	vOutput += vAmbient;
	
	vOutput = vOutput / (vOutput + vec3(1.0));
	vOutput = pow(vOutput, vec3(1.0/2.2));

	return vec4(vOutput, w);
}

void main() {
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bAlbedoMap) != 0)
		vAlbedo = texture(smpAlbedo, vInputUV).rgb;
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bEmissionMap) != 0)
		vEmission = texture(smpEmission, vInputUV).rgb;
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bNormalMap) != 0)
		vNormal = normalize(texture(smpNormal, vInputUV).rgb);
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bMetallicMap) != 0)
		fMetallic = texture(smpMetallic, vInputUV).r;
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bRoughnessMap) != 0)
		fRoughness = texture(smpRoughness, vInputUV).r;
	if ((ssboMaterial.materials[iMaterialIndex].uSamplerBits & bAmbientOcclusionMap) != 0)
		fAmbientOcclusion = texture(smpAmbientOcclusion, vInputUV).r;

	vF0 = mix(vec3(0.04), vAlbedo, fMetallic);
	fAlpha = SQ(fRoughness);
	fK = fAlpha / 2.0;

	vFragColor = PBR();
}