/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: tex_mapped.frag - GLSL vertex shader for 3D texture mapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable


// Output fragment
layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
