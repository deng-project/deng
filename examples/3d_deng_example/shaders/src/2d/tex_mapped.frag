/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: tex_mapped.frag - GLSL fragment shader for 2D texture mapped assets
/// author: Karl-Mihkel Ott


#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D tex_sampler;

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_tex;
layout(location = 2) in flat uint in_is_unmapped;

layout(location = 0) out vec4 out_color;

void main() {
    // Check if the asset is unmapped
    if(in_is_unmapped == 1)
        out_color = in_color;
    else out_color = texture(tex_sampler, in_tex);
}
