#version 450
#extension GL_ARB_separate_shaders : enable

// input
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;

// output
layout(location = 0) out vec2 oTex;

// uniform
layout(binding = 0) uniform CameraTransform {
	mat4 view;
	mat4 projection;
} camera; 

void main() {
	gl_Position = vec4(pos, 1.0f);
	oTex = tex;
}