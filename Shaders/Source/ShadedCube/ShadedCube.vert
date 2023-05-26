#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputPosition;
layout(location = 1) in vec3 vInputNormal;

layout(location = 0) out vec3 vOutputPosition;
layout(location = 1) out vec3 vOutputNormal;

layout(std140, set = 0, binding = 0) uniform Camera {
	mat4 mView;
	mat4 mProjection;
	vec4 vPosition;
} uboCamera;

layout(std140, set = 1, binding = 1) uniform Transform {
	mat4 mCustom;
	vec4 vTranslation;
	vec3 vScale;
	float _pad;
	vec3 vRotation;
	float _pad1;
} uboTransform;

mat4 CalculateRotation() {
	mat4 mX = mat4(1.f);
	mX[1][1] = cos(uboTransform.vRotation.x);
	mX[2][2] = mX[1][1];
	mX[1][2] = sin(uboTransform.vRotation.x);
	mX[2][1] = -mX[1][2];
	
	mat4 mY = mat4(1.f);
	mY[0][0] = cos(uboTransform.vRotation.y);
	mY[2][2] = mY[0][0];
	mY[3][0] = sin(uboTransform.vRotation.y);
	mY[0][3] = -mY[3][0];
	
	mat4 mZ = mat4(1.f);
	mZ[0][0] = cos(uboTransform.vRotation.z);
	mZ[1][1] = mZ[0][0];
	mZ[0][1] = sin(uboTransform.vRotation.z);
	mZ[1][0] = -mZ[0][1];
	
	return mX * mY * mZ;
}

mat4 CalculateTransform() {
	mat4 mTranslation = mat4(1.f);
	mTranslation[3][0] = uboTransform.vTranslation.x;
	mTranslation[3][1] = uboTransform.vTranslation.y;
	mTranslation[3][2] = uboTransform.vTranslation.z;

	mat4 mRotation = CalculateRotation();
	
	mat4 mScale = mat4(1.f);
	mScale[0][0] = uboTransform.vScale.x;
	mScale[1][1] = uboTransform.vScale.y;
	mScale[2][2] = uboTransform.vScale.z;
	
	return mTranslation * mRotation * mScale;
}


void main() {
	mat4 mTransform = CalculateTransform();
	gl_Position = uboCamera.mProjection * uboCamera.mView * mTransform * vec4(vInputPosition, 1.0f);
	vOutputPosition = vec3(mTransform * vec4(vInputPosition, 1.f));
	vOutputNormal = mat3(transpose(inverse(mTransform))) * vInputNormal;
}