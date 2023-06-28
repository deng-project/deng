#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera;

layout(set = 0, binding = 0) uniform Scale {
	vec4 vScale;
} uboScale;

layout(location = 0) in vec3 vInputPosition;
layout(location = 0) out vec3 vOutputUV;

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

mat4 CalculateTransform() {
	mat4 mTransform = mat4(1.f);
	mTransform[3][0] = uboCamera.vPosition.x;
	mTransform[3][1] = uboCamera.vPosition.y;
	mTransform[3][2] = uboCamera.vPosition.z;
	
	return mTransform;
}

void main() {
	mat4 mView = CalculateViewMatrix();
	mat4 mTransform = CalculateTransform();
	gl_Position = uboCamera.mProjection * mView * mTransform * (uboScale.vScale * vec4(vInputPosition, 1.f));
	vOutputUV = vInputPosition;
}