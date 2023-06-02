#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 vFragColor;

void main() {
	vFragColor = vec4(1.f, 1.f, 1.f, 1.f);
}