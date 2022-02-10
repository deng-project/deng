#version 450
#extension GL_ARB_separate_shader_objects : enable

// uniforms 
layout(binding = 0) uniform XRotation {
	mat4 data;
} x_rot;

layout(binding = 1) uniform YRotation {
	mat4 data;
} y_rot;

// input data
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

// output data
layout (location = 0) out vec2 out_tex;

void main() {
	gl_Position = vec4(pos, 1.0f);
	out_tex = tex;
}
