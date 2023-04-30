#version 450
#extension GL_ARB_separate_shader_objects : enable

// macros for controlling different features of the shader are following:
//	 VERTEX_NORMALS=<N> - vertex normal attribute location
//		OUTPUT_VERTEX_NORMALS=<N> - vertex normal output attribute location (default: VERTEX_NORMALS)
//	 VERTEX_TANGENTS=<N> - vertex tangent attribute location
//		OUTPUT_VERTEX_TANGENTS=<N> - vertex tangent output attribute location (default: VERTEX_TANGENTS)
//   UV_COORDINATES=<N> - uv coordinate attribute location
//		UV_ATTRIB_COUNT=<N> - amount of UV attributes
//		OUTPUT_UV_COORDINATES=<N> - UV coordinates output attribute location (default: UV_COORDINATES)
//	 COLOR_MULTIPLIERS=<N> - color multiplier attributes' location
//		COLOR_MULTIPLIER_ATTRIB_COUNT=<N> - amount of color multiplier attributes
//		OUTPUT_COLOR_MULTIPLIERS=<N> - color multiplier attributes' output location
//   USE_JOINT_TRANSFORMS - toggle joint transform usage
//		JOINT_INDICES=<N> - joint indices input location
//		JOINT_WEIGHTS=<N> - joint weights input location
//		JOINT_SET_COUNT=<N> - amount of joint sets to use
//		JOINT_COUNT=<N> - amount of joints to use
//	 USE_MORPH_TARGETS - toggle morph target transformation
//		MORPH_SET_COUNT=<N> - amount of morph weights to use in uniform data
//		MORPH_TARGET_COUNT=<N> - amount of morph targets to use
//      MORPH_POSITIONS=<N> - morph target positions location
//		MORPH_NORMALS=<N> - morph target normals location
//      MORPH_TANGENTS=<N> - morph target tangents location


// vulkan
#if defined(VULKAN)
// NOTE: sets are used for per-mesh / per-shader uniforms
// they are vulkan-specific properties that shouldn't be used anywhere else
layout(std140, set = 0, binding = 0) uniform CameraUniform {
	mat4 mProjection;
	mat4 mView;
	vec4 vPos;
} uCamera;

layout(std140, set = 1, binding = 1) uniform ModelUniform {
	mat4 mNode;
	vec4 vColor;
	uint bIsColor;
	uint bUseEnvironmentMap;
} uModel;

#if defined(USE_JOINT_TRANSFORMS)
layout(std140, binding = 2) uniform JointUniform {
	mat4 mJointMatrices[JOINT_COUNT];
} uJoints;

#if defined(USE_MORPH_TARGETS)
layout(std140, binding = 3) uniform MorphWeightUniform {
	vec4 vMorphWeights[MORPH_SET_COUNT];
} uMorphWeights;
#endif

#elif defined(USE_MORPH_TARGETS)
layout(std140, binding = 2) uniform MorphWeightUniform {
	vec4 vMorphWeights[MORPH_SET_COUNT];
} uMorphWeights;
#endif

// opengl
#else
layout(std140, binding = 0) uniform CameraUniform {
	mat4 mProjection;
	mat4 mView;
	vec4 vPos;
} uCamera;

layout(std140, binding = 1) uniform ModelUniform {
	mat4 mNode;
	vec4 vColor;
	uint bIsColor;
	uint bUseEnvironmentMap;
} uModel;

#if defined(USE_JOINT_TRANSFORMS)
layout(std140, binding = 2) uniform JointUniform {
	mat4 mJointMatrices[JOINT_COUNT];
} uJoints;

#if defined(USE_MORPH_TARGETS)
layout(std140, binding = 3) uniform MorphWeightUniform {
	vec4 vMorphWeights[MORPH_SET_COUNT];
} uMorphWeights;
#endif

#elif defined(USE_MORPH_TARGETS)
layout(std140, binding = 2) uniform MorphWeightUniform {
	vec4 vMorphWeights[MORPH_SET_COUNT];
} uMorphWeights;
#endif

#endif

// set output properties if they are not explicitly set
#if defined(VERTEX_NORMALS) && !defined(OUTPUT_VERTEX_NORMALS)
#define OUTPUT_VERTEX_NORMALS VERTEX_NORMALS
#endif

#if defined(VERTEX_TANGENTS) && !defined(OUTPUT_VERTEX_TANGENTS)
#define OUTPUT_VERTEX_TANGENTS VERTEX_TANGENTS
#endif

#if defined(UV_COORDINATES) && !defined(OUTPUT_UV_COORDINATES)
#define OUTPUT_UV_COORDINATES UV_COORDINATES
#endif

#if defined(COLOR_MULTIPLIERS) && !defined(OUTPUT_COLOR_MULTIPLIERS)
#define OUTPUT_COLOR_MULTIPLIERS COLOR_MULTIPLIERS
#endif

layout(location = 0) in vec3 vInputPosition;
layout(location = 0) out vec3 vOutputPosition;

#if defined(VERTEX_NORMALS)
layout(location = VERTEX_NORMALS) in vec3 vInputNormal;
layout(location = OUTPUT_VERTEX_NORMALS) out vec4 vOutputNormal;
#endif

#if defined(VERTEX_TANGENTS)
layout(location = VERTEX_TANGENTS) in vec4 vInputTangent;
layout(location = OUTPUT_VERTEX_TANGENTS) out vec4 vOutputTangents;
#endif

#if defined(UV_COORDINATES) && defined(UV_ATTRIB_COUNT)
layout(location = UV_COORDINATES) in vec2 vInputUVs[UV_ATTRIB_COUNT];
layout(location = OUTPUT_UV_COORDINATES) out vec2 vOutputUVs[UV_ATTRIB_COUNT];
#endif

#if defined(COLOR_MULTIPLIERS) && defined(COLOR_MULTIPLIER_ATTRIB_COUNT)
layout(location = COLOR_MULTIPLIERS) in vec4 vInputColorMultipliers[COLOR_MULTIPLIER_ATTRIB_COUNT];
layout(location = OUTPUT_COLOR_MULTIPLIERS) out vec4 vOutputColorMultipliers[COLOR_MULTIPLIER_ATTRIB_COUNT];
#endif

#ifdef USE_JOINT_TRANSFORMS
layout (location = JOINT_INDICES) in uvec4 vInputJointIndices[JOINT_SET_COUNT];
layout (location = JOINT_WEIGHTS) in vec4 vInputJointWeights[JOINT_SET_COUNT];
#endif

#ifdef USE_MORPH_TARGETS

layout(location = MORPH_POSITIONS) in vec3 vInputMorphPositions[MORPH_TARGET_COUNT];

#if defined(VERTEX_NORMALS) && defined(MORPH_NORMALS)
layout(location = MORPH_NORMALS) in vec3 vInputMorphNormals[MORPH_TARGET_COUNT];
#endif

#if defined(VERTEX_TANGENTS) && defined(MORPH_TANGENTS)
layout(location = MORPH_TANGENTS) in vec3 vInputMorphTangents[MORPH_TARGET_COUNT];
#endif

#endif

void main() {
	const mat4 mCamera = uCamera.mProjection * uCamera.mView;
	mat4 mCustomTransforms = mat4(1.0f);
	vec3 vPosition = vInputPosition;

#ifdef USE_VERTEX_NORMALS
	vec3 vNormal = vInputNormal;
#endif

#ifdef USE_VERTEX_TANGENTS
	vec4 vTangent = vInputTangent;
#endif

	// morph target calculations
#if defined(USE_MORPH_TARGETS)
	for (int i = 0; i < MORPH_TARGET_COUNT; i++) {
		switch(i % 4) {
			case(0):
				vPosition += vInputMorphPositions[i] * uMorphWeights.vMorphWeights[i / 4].x;
				break;

			case(1):
				vPosition += vInputMorphPositions[i] * uMorphWeights.vMorphWeights[i / 4].y;
				break;

			case(2):
				vPosition += vInputMorphPositions[i] * uMorphWeights.vMorphWeights[i / 4].z;
				break;

			case(3):
				vPosition += vInputMorphPositions[i] * uMorphWeights.vMorphWeights[i / 4].w;
				break;
		}
	}

#if defined(VERTEX_NORMALS) && defined(MORPH_NORMALS)
	for (int i = 0; i < MORPH_TARGET_COUNT; i++) {
		switch(i % 4) {
			case(0):
				vNormal += vInputMorphNormals[i] * uMorphWeights.vMorphWeights[i / 4].x;
				break;

			case(1):
				vNormal += vInputMorphNormals[i] * uMorphWeights.vMorphWeights[i / 4].y;
				break;

			case(2):
				vNormal += vInputMorphNormals[i] * uMorphWeights.vMorphWeights[i / 4].z;
				break;

			case(3):
				vNormal += vInputMorphNormals[i] * uMorphWeights.vMorphWeights[i / 4].w;
				break;
		}
	}
#endif

#if defined(VERTEX_TANGENTS) && defined(MORPH_TANGENTS)
	for (int i = 0; i < MORPH_TARGET_COUNT; i++) {
		switch(i % 4) {
			case(0):
				vTangent += vInputMorphTangents[i] * uMorphWeights.vMorphWeights[i / 4].x;
 				break;

			case(1):
				vTangent += vInputMorphTangents[i] * uMorphWeights.vMorphWeights[i / 4].y;
				break;

			case(2):
				vTangent += vInputMorphTangents[i] * uMorphWeights.vMorphWeights[i / 4].z;
				break;

			case(3):
				vTangent += vInputMorphTangents[i] * uMorphWeights.vMorphWeights[i / 4].w;
				break;
		}
	}
#endif

#endif

	// joint transforms
#ifdef USE_JOINT_TRANSFORM
	for (int i = 0; i < JOINT_SET_COUNT; i++) {
		mCustomTransforms += 
			vInputJointWeights[i].x * uJoints.mJointMatrices[int(vInputJointIndices[i].x)] +
			vInputJointWeights[i].y * uJoints.mJointMatrices[int(vInputJointIndices[i].y)] +
			vInputJointWeights[i].z * uJoints.mJointMatrices[int(vInputJointIndices[i].z)] +
			vInputJointWeights[i].w * uJoints.mJointMatrices[int(vInputJointIndices[i].w)];
	}
#endif

	// apply transforms to vertex tangent and normal if possible
#ifdef USE_VERTEX_NORMALS
	vOutputNormal = mCamera * uModel.mNode * mCustomTransforms * vec4(vNormal, 1.0f);
	vOutputNormal = normalize(vOutputNormal);
#endif

#ifdef USE_VERTEX_TANGENTS
	vOutputTangent = mCamera * uModel.mNode * mCustomTransforms * vTangent;
	vOutputTangent = normalize(vOutputTangent);
#endif

	gl_Position = mCamera * uModel.mNode * mCustomTransforms * vec4(vPosition, 1.0f);
	vOutputPosition = gl_Position.xyz;

	// set output color color attributes
#ifdef USE_COLOR_MULTIPLIERS
	for (int i = 0; i < COLOR_MULTIPLIER_ATTRIB_COUNT; i++) 
		vOutputColorMultiplierAttributes[i] = vInputColorMultiplierAttributes[i];
#endif

#ifdef USE_UV_COORDINATES
	for (int i = 0; i < COLOR_MULTIPLIER_ATTRIB_COUNT; i++)
		vOutputUVAttributes[i] = vInputUVAttributes[i];
#endif
}