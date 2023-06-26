#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;
layout(location = 2) in vec2 vInputUV;

layout(location = 0) out vec3 vOutputPosition;
layout(location = 1) out vec3 vOutputNormal;
layout(location = 2) out vec2 vOutputUV;
layout(location = 3) out flat int vOutputIndex;

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera;


struct Transform {
	mat4 mCustom;
	mat4 mNormal;
	vec4 vTranslation;
	vec4 vScale;
	vec4 vRotation;
};

struct DrawDescriptorIndices {
	ivec4 indices;
};

layout(std430, set = 0, binding = 0) readonly buffer DrawDescriptorIndicesSSBO {
	DrawDescriptorIndices descriptors[];
} uboIndices;

layout(std430, set = 0, binding = 1) readonly buffer TransformSSBO {
	Transform transforms[];
} uboTransform;

mat4 CalculateRotation() {
	const int ciIndex = uboIndices.descriptors[gl_InstanceIndex].indices.x;
	
	mat4 mX = mat4(1.f);
	mX[1][1] = cos(uboTransform.transforms[ciIndex].vRotation.x);
	mX[2][2] = mX[1][1];
	mX[1][2] = sin(uboTransform.transforms[ciIndex].vRotation.x);
	mX[2][1] = -mX[1][2];
	
	mat4 mY = mat4(1.f);
	mY[0][0] = cos(uboTransform.transforms[ciIndex].vRotation.y);
	mY[2][2] = mY[0][0];
	mY[2][0] = sin(uboTransform.transforms[ciIndex].vRotation.y);
	mY[0][2] = -mY[2][0];
	
	mat4 mZ = mat4(1.f);
	mZ[0][0] = cos(uboTransform.transforms[ciIndex].vRotation.z);
	mZ[1][1] = mZ[0][0];
	mZ[0][1] = sin(uboTransform.transforms[ciIndex].vRotation.z);
	mZ[1][0] = -mZ[0][1];
	
	return mX * mY * mZ;
}

mat4 CalculateTransform() {
	const int ciIndex = uboIndices.descriptors[gl_InstanceIndex].indices.x;
	
	mat4 mTranslation = mat4(1.f);
	mTranslation[3][0] = uboTransform.transforms[ciIndex].vTranslation.x;
	mTranslation[3][1] = uboTransform.transforms[ciIndex].vTranslation.y;
	mTranslation[3][2] = uboTransform.transforms[ciIndex].vTranslation.z;

	mat4 mRotation = CalculateRotation();
	
	mat4 mScale = mat4(1.f);
	mScale[0][0] = uboTransform.transforms[ciIndex].vScale.x;
	mScale[1][1] = uboTransform.transforms[ciIndex].vScale.y;
	mScale[2][2] = uboTransform.transforms[ciIndex].vScale.z;
	
	return mTranslation * mRotation * mScale;
}


mat4 CalculateViewMatrix() {
	mat4 mLookAt = mat4(1.f);
	mLookAt[0][0] = uboCamera.vCameraRight.x;
	mLookAt[1][0] = uboCamera.vCameraRight.y;
	mLookAt[2][0] = uboCamera.vCameraRight.z;
	
	mLookAt[0][1] = uboCamera.vCameraUp.x;
	mLookAt[1][1] = uboCamera.vCameraUp.y;
	mLookAt[2][1] = uboCamera.vCameraUp.z;
	
	mLookAt[0][2] = uboCamera.vCameraLookAt.x;
	mLookAt[1][2] = uboCamera.vCameraLookAt.y;
	mLookAt[2][2] = uboCamera.vCameraLookAt.z;
	
	mat4 mTranslation = mat4(1.f);
	mTranslation[3].xyz = -uboCamera.vPosition.xyz;
	
	return mLookAt * mTranslation;
}


void main() {
	const mat4 mTransform = CalculateTransform();
	const mat4 mView = CalculateViewMatrix();
	const int ciIndex = uboIndices.descriptors[gl_InstanceIndex].indices.x;
	
	gl_Position = uboCamera.mProjection * mView * mTransform * vec4(vInputPosition, 1.0f);
	vOutputPosition = vec3(mTransform * vec4(vInputPosition, 1.f));
	vOutputNormal = mat3(uboTransform.transforms[ciIndex].mNormal) * vInputNormal;
	vOutputUV = vInputUV;
	vOutputIndex = gl_InstanceIndex;
}