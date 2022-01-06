/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: unmapped.frag - GLSL fragment shader for 3D unmapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable

// All the data that is passed to the fragment shader
layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
