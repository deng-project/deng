/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#version 450
#extension GL_ARB_separate_shader_objects : enable


const int max_light_src_c = 8;


struct Light {
    vec4 pos;
    float intensity;
    uint pad1;
    vec2 pad2;
};

/// Uniform object for containing data about the asset
layout(std140, binding = 1) uniform AssetData {
    vec4 kd;
    vec4 ka;
    vec4 ks;
    float phong_exp;
    uint ignore_transform;
    uint is_unmapped;

    // Padding
    uint pad;
} asset_data;


/// Uniform buffer of all lighting objects to use
layout(std140, binding = 2) uniform LightData {
    // Light source data
    Light light[max_light_src_c];
    uint light_src_c;

    // Padding
    vec2 pad1;
    uint pad2;
} ld;

layout(binding = 3) uniform sampler2D tex_sampler;


// Fragment input data
layout(location = 0) in vec2 in_tex;
layout(location = 1) in vec3 in_vert;
layout(location = 2) in vec4 in_pos;
layout(location = 3) in vec3 in_normal;

// Output fragment
layout(location = 0) out vec4 out_color;

void main() {
    vec4 dif_color;

    // Check if color or texture mapping should be used
    if(asset_data.is_unmapped == 1)
        dif_color = asset_data.kd;
    else dif_color = texture(tex_sampler, in_tex);

    out_color = asset_data.ka * dif_color;

    // For each light source apply Blinn-Phong shading
    for(uint i = 0; i < ld.light_src_c; i++) {
        vec3 light_ray = normalize(vec3(ld.light[i].pos) - in_vert);

        // Cosine of the angle between surface to light position vector and 
        // vertex normal vector
        float cos_theta = dot(light_ray, normalize(in_normal));
        
        // Calculate the half vector between surface to light source and 
        // surface to view vectors
        vec3 hf = normalize(vec3(in_pos) + light_ray);

        // Calculate the cosine of the angle between vertex normal and 
        // half vector
        float cos_alpha = dot(normalize(in_normal), hf);

        out_color += dif_color * ld.light[i].intensity * clamp(cos_theta, 0, 1) +
            asset_data.ks * ld.light[i].intensity * pow(clamp(cos_alpha, 0, 1), asset_data.phong_exp);
    }
}