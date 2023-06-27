#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(points) out;
layout(max_vertices = 1) out;

void main() {
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}