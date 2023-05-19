#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inputColor;
layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = inputColor;
}