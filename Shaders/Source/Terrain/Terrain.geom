#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
layout(location = 0) out vec4 vOutputPosition;
layout(location = 1) out vec2 vOutputUV;

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera;

layout(set = 0, binding = 0) uniform sampler2D smpHeight;

// constant definitions
#define STEP 0.2f
#define MAX_HEIGHT 5.f
#define BOUNDS 20.f
#define PI 3.141592653589793f

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

float SampleHeight(in vec2 vPosition) {
	vec2 vUV = vec2((vPosition.x + BOUNDS) / (2*BOUNDS), (vPosition.y + BOUNDS) / (2*BOUNDS));
	vUV = clamp(vUV, 0.0f, 1.f);
	vec4 vHeight = texture(smpHeight, vUV);
	return vHeight.x;
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

mat4 rotationX(in float fRad) {
	mat4 mRot = mat4(1.f);
	mRot[1][1] = cos(fRad);
	mRot[2][2] = mRot[1][1];
	mRot[1][2] = sin(fRad);
	mRot[2][1] = -mRot[1][2];
	return mRot;
}

mat4 rotationY(in float fRad) {
	mat4 mRot = mat4(1.f);
	mRot[0][0] = cos(fRad);
	mRot[2][2] = mRot[0][0];
	mRot[2][0] = sin(fRad);
	mRot[0][2] = -mRot[2][0];
	return mRot;
}

mat4 rotationZ(in float fRad) {
	mat4 mRot = mat4(1.f);
	mRot[0][0] = cos(fRad);
	mRot[1][1] = mRot[0][0];
	mRot[0][1] = sin(fRad);
	mRot[1][0] = -mRot[0][1];
	return mRot;
}

void CreateQuad(mat4 mView, vec4 vBase) {
	if (vBase.x > BOUNDS - STEP || vBase.z > BOUNDS - STEP)
		return;
		
	vec4 vPosition[4];
	vPosition[0] = vBase;								// bottom left
	vPosition[1] = vec4(0.f, 0.f, STEP, 0.f) + vBase;	// bottom right
	vPosition[2] = vec4(STEP, 0.f, 0.f, 0.f) + vBase; 	// top left
	vPosition[3] = vec4(STEP, 0.f, STEP, 0.f) + vBase; 	// top right
	
	vec4 vUVCoords[4];
	mat4 rotZ = rotationZ(random(vBase.xz)*PI);
	vUVCoords[0] = rotZ * vec4(0.f, 1.f, 0.f, 0.f);
	vUVCoords[1] = rotZ * vec4(1.f, 1.f, 0.f, 0.f);
	vUVCoords[2] = rotZ * vec4(0.f, 0.f, 0.f, 0.f);
	vUVCoords[3] = rotZ * vec4(1.f, 0.f, 0.f, 0.f);
	
	for (int i = 0; i < 4; i++) {
		float fHeight = SampleHeight(vPosition[i].xz);
		vPosition[i].y = MAX_HEIGHT * fHeight;
		
		gl_Position = uboCamera.mProjection * mView * vPosition[i];
		vOutputPosition = vPosition[i];
		vOutputUV = vUVCoords[i].xy;
		EmitVertex();
	}
	EndPrimitive();
}

void main() {
	mat4 mView = CalculateViewMatrix();
	CreateQuad(mView, gl_in[0].gl_Position);
}