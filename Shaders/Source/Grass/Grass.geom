#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(triangle_strip, max_vertices = 12) out;
layout(location = 0) out vec2 vOutputUV;

layout(push_constant) uniform Camera {
	mat4 mProjection;
	vec4 vCameraRight;
	vec4 vCameraUp;
	vec4 vCameraLookAt;
	vec4 vPosition;
} uboCamera;

layout(set = 0, binding = 1) uniform sampler2D smpWind;
layout(set = 0, binding = 2) uniform sampler2D smpHeight;
layout(set = 0, binding = 3) uniform Wind {
	float fTime;
} uboTime;

// constants
#define PI 3.141592653589793f
#define MIN_SIZE 0.4f
#define BOUNDS 20.f
#define MAX_HEIGHT 5.f
#define WIND_STRENGTH 0.05f

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

float random (vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

float SampleHeight(in vec2 vPosition) {
	vec2 vUV = vec2((vPosition.x + BOUNDS) / (2*BOUNDS), (vPosition.y + BOUNDS) / (2*BOUNDS));
	vUV = clamp(vUV, 0.0f, 1.f);
	vec4 vHeight = texture(smpHeight, vUV);
	return vHeight.x;
}

mat4 CalculateWind(vec3 vBase) {
	const vec2 vWindDirection = vec2(1.f, 1.f);
	vec2 vUV = vBase.xz / 10.f + vWindDirection * WIND_STRENGTH * uboTime.fTime;
	vUV.x = mod(vUV.x, 1.f);
	vUV.y = mod(vUV.y, 1.f);
	
	vec4 vWind = texture(smpWind, vUV);
	return (rotationX(vWind.x * PI * 0.75f - PI * 0.25f) * rotationZ(vWind.y * PI * 0.75f - PI * 0.25f));
}

void CreateQuad(mat4 mView, vec4 vBase, mat4 mModel) {
	vec4 vPositions[4];
	vPositions[0] = vec4(-0.3f, 0.f, 0.f, 0.f);
	vPositions[1] = vec4(0.3f, 0.f, 0.f, 0.f);
	vPositions[2] = vec4(-0.3f, 0.7f, 0.f, 0.f);
	vPositions[3] = vec4(0.3f, 0.7f, 0.f, 0.f);
	
	// UV coordinates
	vec2 vUVCoords[4];
	vUVCoords[0] = vec2(0.f, 1.f);
	vUVCoords[1] = vec2(1.f, 1.f);
	vUVCoords[2] = vec2(0.f, 0.f);
	vUVCoords[3] = vec2(1.f, 0.f);
	
	mat4 randY = rotationY(random(vBase.zx)*PI);
	float fGrassSize = random(vBase.xz) * (1.f - MIN_SIZE) + MIN_SIZE;
	
	mat4 mWind = CalculateWind(vBase.xyz);
	
	vec4 vTranslation = vec4(0.f, MAX_HEIGHT * SampleHeight(vBase.xz), 0.f, 0.f);
	
	// temporary
	if (vTranslation.y > 2.0f)
		return;
	
	for (int i = 0; i < 4; i++) {
		mat4 mWindApply = mat4(1.f);
		if (i >= 2) mWindApply = mWind;
		
		vec4 vGrass = vTranslation + vBase + (mWindApply * randY * mModel * (fGrassSize * vPositions[i]));
		gl_Position = uboCamera.mProjection * mView * vGrass;
		vOutputUV = vUVCoords[i];
		EmitVertex();
	}
	EndPrimitive();
}


void main() {
	mat4 mView = CalculateViewMatrix();
	mat4 model0, model45, modelm45;
	model0 = mat4(1.f);
	model45 = rotationY(radians(45));
	modelm45 = rotationY(-radians(45));
	
	CreateQuad(mView, gl_in[0].gl_Position, model0);
	CreateQuad(mView, gl_in[0].gl_Position, model45);
	CreateQuad(mView, gl_in[0].gl_Position, modelm45);
}