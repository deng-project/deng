#version 450
#extension GL_ARB_separate_shader_objects : enable

// input data
layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec2 i_uv;
layout(location = 2) in vec3 i_norm;
layout(location = 3) in vec4 i_tang;

// output data
layout(location = 0) out vec2 o_uv;

// Camera uniform data
layout(binding = 0) uniform CameraUbo {
    mat4 projection;
    mat4 view;
} camera;


// Animation uniform data
#define TARGET_MASK_WEIGHT              0x00000001
#define TARGET_MASK_ROTATION            0x00000002
#define TARGET_MASK_TRANSLATION         0x00000004
#define TARGET_MASK_SCALE               0x00000008

#define INTERPOLATION_TYPE_LINEAR       0
#define INTERPOLATION_TYPE_STEP         1
#define INTERPOLATION_TYPE_CUBICSPLINE  2

layout(binding = 1) uniform AnimationUbo {
    vec4 rotations[2];      // quaternions
    vec4 weights[2];        // point structures
    vec4 translations[2];   // point structures
    float scales[2];        // uniform scales

    uint target_mask; 
    uint interpolation_mode;
    uint animate;

    float timestamps[2];
    float current_time;
    uint used_weights;
} animation;


// Model uniform data
layout(binding = 2) uniform ModelUbo {
    mat4 node;
    mat4 skeleton;
} model;

/*** Quaternion operations ***/

vec4 QConjugate(vec4 _q1) {
    return vec4(-_q1.xyz, _q1.w);
}


vec4 QInverse(vec4 _q1) {
    vec4 conj = QConjugate(_q1);
    float len = _q1.length();
    return conj / (len * len);
}


vec4 QGrassman(vec4 _p, vec4 _q) {
    return vec4(vec3(_p.w * _q.xyz + _q.w * _p.xyz + cross(_p.xyz, _q.xyz)), (_p.w * _q.w - dot(_p.xyz, _q.xyz)));
}


mat4 QToMatrix(vec4 _q) {
    float x2 = _q.x * _q.x;
    float y2 = _q.y * _q.y;
    float z2 = _q.z * _q.z;

    float xy = _q.x * _q.y;
    float xz = _q.x * _q.z;
    float xw = _q.x * _q.w;

    float yz = _q.y * _q.z;
    float yw = _q.y * _q.w;

    float zw = _q.z * _q.w;

    //mat4 m = mat4(
        //(1 - 2 * y2 - 2 * z2), (2 * xy - 2 * zw), (2 * xz - 2 * yw), 0,
        //(2 * xy + 2 * zw), (1 - 2 * x2 - 2 * z2), (2 * yz - 2 * xw), 0,
        //(2 * xz - 2 * yw), (2 * yz + 2 * xw), (1 - 2 * x2 - 2 * y2), 0,
        //0, 0, 0, 1
    //);

    mat4 m = mat4(
        (1 - 2 * y2 - 2 * z2), (2 * xy + 2 * zw), (2 * xz - 2 * yw), 0,
        (2 * xy - 2 * zw), (1 - 2 * x2 - 2 * z2), (2 * yz + 2 * xw), 0,
        (2 * xz - 2 * yw), (2 * yz - 2 * xw), (1 - 2 * x2 - 2 * y2), 0,
        0, 0, 0, 1
    );

    return m;
}

/*** End of quaternion operations ***/


/*** Animation functions ***/

// interpolate target value using linear interpolation
mat4 LinearInterpolation() {
    mat4 interp_mat = mat4(1.0f);
    float t = (animation.current_time - animation.timestamps[0]) / (animation.timestamps[1] - animation.timestamps[0]);

    switch(animation.target_mask) {
        // not supported
        case TARGET_MASK_WEIGHT:
            break;

        case TARGET_MASK_ROTATION:
            {
                // first keyframe
                float rot1 = acos(animation.rotations[0].w) * 2;
                float sinf1 = sin(rot1 / 2);
                vec3 scale1 = animation.rotations[0].xyz / sinf1;

                float rot2 = acos(animation.rotations[1].w) * 2;
                float sinf2 = sin(rot2 / 2);
                vec3 scale2 = animation.rotations[1].xyz / sinf2;

                float irot = (1 - t) * rot1 + t * rot2;
                vec3 iscale = (1 - t) * scale1 + t * scale2;

                mat4 x = mat4(
                    1, 0, 0, 0,
                    0, cos(irot * iscale.x), sin(irot * iscale.x), 0,
                    0, -sin(irot * iscale.x), cos(irot * iscale.x), 0,
                    0, 0, 0, 1
                );

                mat4 y = mat4(
                    cos(irot * iscale.y), 0, -sin(irot * iscale.y), 0,
                    0, 1, 0, 0,
                    sin(irot * iscale.y), 0, cos(irot * iscale.y), 0,
                    0, 0, 0, 1
                );

                mat4 z = mat4(
                    cos(irot * iscale.z), sin(irot * iscale.z), 0, 0,
                    -sin(irot * iscale.z), cos(irot * iscale.z), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
                );

                interp_mat *= z * y * x;
            }
            break;

        case TARGET_MASK_TRANSLATION:
            {
                vec4 itranslation = (1 - t) * animation.translations[0] + t * animation.translations[1];
                interp_mat[3][0] = itranslation.x;
                interp_mat[3][1] = itranslation.y;
                interp_mat[3][2] = itranslation.z;
            }
            break;

        case TARGET_MASK_SCALE:
            {
                float iscale = (1 - t) * animation.scales[0] + t * animation.scales[1];
                interp_mat[0][0] = iscale;
                interp_mat[1][1] = iscale;
                interp_mat[2][2] = iscale;
            }
            break;
    }

    return interp_mat;
}


mat4 StepInterpolation() {
    mat4 interp_mat = mat4(1.0f);

    switch(animation.target_mask) {
        case TARGET_MASK_WEIGHT:
            // not supported
            break;

        case TARGET_MASK_ROTATION:
            interp_mat = QToMatrix(animation.rotations[0]);
            break;

        case TARGET_MASK_TRANSLATION:
            interp_mat[3][0] = animation.translations[0].x;
            interp_mat[3][1] = animation.translations[0].y;
            interp_mat[3][2] = animation.translations[0].z;
            break;

        case TARGET_MASK_SCALE:
            interp_mat[0][0] = animation.scales[0];
            interp_mat[1][1] = animation.scales[0];
            interp_mat[2][2] = animation.scales[0];
            break;

        default:
            break;
    }

    return interp_mat;
}


mat4 CalculateAnimation() {
    mat4 ani_mat = mat4(1.0f);
    float t = (animation.current_time - animation.timestamps[0]) / (animation.timestamps[1] - animation.timestamps[0]);
        
    // for now only linear and step interpolations are supported
    if(animation.animate == 1) {
        switch(animation.interpolation_mode) {
            case INTERPOLATION_TYPE_LINEAR:
                ani_mat = LinearInterpolation();
                break;

            case INTERPOLATION_TYPE_CUBICSPLINE:
                // not supported yet
                break;
            
            case INTERPOLATION_TYPE_STEP:
                ani_mat = StepInterpolation();
                break;

            default:
                break;
        }
    }

    return ani_mat;
}

/*** End of animation functions ***/


void main() {
    // TODO: Calculate transformation matrices for each uniform block
    mat4 a = CalculateAnimation();
    mat4 c = camera.projection * camera.view;
    mat4 m = model.skeleton * model.node;

    gl_Position = a * c * m * vec4(i_pos, 1.0f);
    o_uv = i_uv;
}
