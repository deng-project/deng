#include <deng/deng.h>
using namespace dengMath;

int main(void) {
    deng_InitWindowAPI();

    {
        // Create Window and Camera3D class objects
        deng::Window win = deng::Window(1280, 720, DENG_WINDOW_HINT_API_OPENGL | DENG_WINDOW_HINT_FIXED_SIZE, "DENG example");
        deng::Camera3D cam = deng::Camera3D(DENG_CAMERA_TYPE_FPP, static_cast<deng_vec_t>(conversion::degToRad(65.0)), vec2<deng_vec_t>(-0.1f, -25.0f), 
                                            vec3<deng_vec_t>(0.7f, 0.7f, 0.7f), vec2<deng_f64_t>(0.3f, 0.3f), &win);

        // Create keybindings for the camera system
        deng::Camera3DBindings bindings;
        bindings.mov_u = deng_CreateInputMask(1, NEKO_KEY_D);
        bindings.mov_nu = deng_CreateInputMask(1, NEKO_KEY_A);
        bindings.mov_v = deng_CreateInputMask(1, NEKO_KEY_SPACE);
        bindings.mov_nv = deng_CreateInputMask(1, NEKO_KEY_LEFT_CTRL);
        bindings.mov_w = deng_CreateInputMask(1, NEKO_KEY_S);
        bindings.mov_nw = deng_CreateInputMask(1, NEKO_KEY_W);

        bindings.rot_u = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_NY);
        bindings.rot_nu = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_Y);
        bindings.rot_v = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_NX);
        bindings.rot_nv = deng_CreateInputMask(1, NEKO_MOUSE_DELTA_X);
        bindings.ch_vcp = deng_CreateInputMask(1, NEKO_KEY_ESCAPE);
        cam.setBindings(bindings);

        // Load texture information from file
        das_Texture texture = {};
        das_LoadTexture(&texture, "example_tex.tga");

        // Load asset information from file
        das_Asset asset = {};
        das_LoadAsset(&asset, DAS_ASSET_MODE_UNDEFINED, {1.0f, 1.0f, 1.0f, .5f}, 
                      false, texture.uuid, "example_model.das");

        deng::Renderer rend = deng::Renderer(DENG_RENDERER_HINT_MIPMAP_ENABLE | DENG_RENDERER_HINT_MSAA_4, 
                                             vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f));
        rend.submitTexture(texture);
        rend.submitAsset(asset);
        rend.setup(cam, win);
        rend.run();
    }

    deng_DeinitWindowAPI();
}
