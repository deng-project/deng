#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(std140 binding = 0) uniform CameraUbo {
	mat4 projection;
	mat4 view;
} camera;

layout(location = 0) in vec3 in_vert;
layout(location = 0) out vec3 out_uv;

void main() {
	gl_Position = camera.projection * camera.view * vec4(in_vert, 1.0f);
	out_uv = in_vert;
}