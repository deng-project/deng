#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vInputVertex;
layout(location = 1) in vec4 vInputColor;
layout(location = 0) out vec4 vOutputColor;

void main() {
    gl_Position = vec4(vInputVertex, 1.f);
	vOutputColor = vInputColor;
}
