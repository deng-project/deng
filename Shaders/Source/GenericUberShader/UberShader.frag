#version 450
#extension GL_ARB_separate_shader_objects : enable

#ifdef VULKAN
layout (std140, set = 0, binding = 0) uniform CameraUbo {
	mat4 mProjection;
	mat4 mView;
	vec4 vPosition;
} uCamera;

layout (std140, set = 1, binding = 1) uniform ModelUbo {
	mat4 mNode;
	vec4 vColor;
	uint bIsColor;
	uint bUseEnvironmentMap;
} uModel;

#else
layout (std140, binding = 0) uniform CameraUbo {
	mat4 mProjection;
	mat4 mView;
	vec4 vPosition;
} uCamera;

layout (std140, binding = 1) uniform ModelUbo {
	mat4 mNode;
	vec4 vColor;
	uint bIsColor;
	uint bUseEnvironmentMap;
} uModel;
#endif

layout(location = 0) in vec3 vInputPosition;

#if defined(VERTEX_NORMALS)
layout (location = VERTEX_NORMALS) in vec3 vInputNormal;
#elif defined(VERTEX_TANGENTS)
layout (location = VERTEX_TANGENTS) in vec3 vInputTangent;
#endif

#if defined(UV_COORDINATES) && defined(UV_ATTRIB_COUNT)
layout (location = UV_COORDINATES) in vec2 vInputUVs[UV_ATTRIB_COUNT];

#ifdef VULKAN
layout(set = 1, binding = 2) uniform sampler2D u2DTextureSamplers[UV_ATTRIB_COUNT];
#else
layout(binding = 2) uniform sampler2D u2DTextureSamplers[UV_ATTRIB_COUNT];
#endif

#endif

#if defined(COLOR_MULTIPLIERS) && defined(COLOR_MULTIPLIER_ATTRIB_COUNT)
layout (location = COLOR_MULTIPLIERS) in vec4 vInputColorMultipliers[COLOR_MULTIPLIER_ATTRIB_COUNT];
#endif

#ifdef VULKAN
layout (set = 1, binding = 3) uniform samplerCube uEnvironmentCube;
#else
layout (binding = 3) uniform samplerCube uEnvironmentCube;
#endif

layout (location = 0) out vec4 vOutputColor;

void main() {
	if (uModel.bIsColor == 1 && uModel.bUseEnvironmentMap == 0)
		vOutputColor = uModel.vColor;
	else if (uModel.bIsColor == 0 && uModel.bUseEnvironmentMap == 0) {
#if defined(UV_COORDINATES) && defined(COLOR_MULTIPLIERS)
		for (int i = 0; i < min(UV_ATTRIB_COUNT, COLOR_MULTIPLIER_ATTRIB_COUNT); i++)
			vOutputColor += texture(u2DTextureSamplers[i], vInputUVs[i]) * vInputColorMultipliers[i];
#elif defined(UV_COORDINATES)
		const float fScaleFactor = 1.0f / UV_ATTRIB_COUNT
		for (int i = 0; i < UV_ATTRIB_COUNT; i++) {		
			vOutputColor += fScaleFactor * texture(u2DTextureSamplers[i], vInputUVs[i]);
		}
#else
		vOutputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
#endif
	}
	else {
#if defined(VERTEX_NORMALS)
		vec3 I = normalize(vInputPosition - uCamera.vPosition.rgb);
		vec3 R = reflect(I, normalize(vInputNormal));
		vOutputColor = vec4(texture(uEnvironmentCube, R).rgb, 1.f);
#else
		vOutputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
#endif
	}
}