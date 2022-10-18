#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_uv;
layout(location = 1) out vec4 out_frag;
layout(binding = 1) uniform samplerCube skybox;

void main() {
	out_frag = texture(skybox, in_uv);
}