/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: opengl_sandbox.cpp - OpenGL test application implementation
/// author: Karl-Mihkel Ott

#define __OPENGL_SANDBOX_CPP
#include <../src/sandbox/opengl/opengl_sandbox.h>


namespace deng {

    OpenGLApplication::OpenGLApplication() : 
        m_win(1280, 720, DENG_WINDOW_HINT_API_OPENGL | DENG_WINDOW_HINT_FIXED_SIZE, "OpenGL sample"),
        m_cam(DENG_CAMERA_TYPE_EDITOR, dengMath::Conversion::degToRad(65.0), dengMath::vec2<deng_vec_t>{0.1f, -25.0f}, 
            dengMath::vec3<deng_vec_t>{0.7f, 0.7f, 0.7f}, dengMath::vec2<deng_f64_t>{0.3, 0.3}, false, &m_win) {

        // Check the current camera type and create input bindings accordingly
        switch(m_cam.getType()) {
        case DENG_CAMERA_TYPE_FPP:
            __bindFPP();
            break;

        case DENG_CAMERA_TYPE_EDITOR:
            __bindEditor();
            break;

        default:
            break;
        }

        // Set config variables
        m_cfg.background = dengMath::vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
        m_cfg.p_cam = &m_cam;
        m_cfg.p_win = &m_win;
        
        m_rend = std::make_unique<opengl::__gl_Renderer>(m_cfg, m_reg, m_assets, m_textures);

        das_RewriteAssetLogs();
        das_RewriteTextureLogs();

        // Load asset and texture files
        RegData reg_tex = {};
        das_LoadTexture(&reg_tex.tex, "textures/viking_room.tga");

        RegData reg_asset = {};
        das_LoadAsset(&reg_asset.asset, DAS_ASSET_MODE_UNDEFINED, das_ObjColorData{0.2f, 0.7f, 0.0f, 1.0f}, false, 
            reg_tex.tex.uuid, "assets/viking.das");
        reg_asset.asset.force_unmap = false;

        m_reg.push(reg_asset.asset.uuid, DENG_REGISTRY_TYPE_ASSET, reg_asset);
        m_reg.push(reg_tex.tex.uuid, DENG_REGISTRY_TYPE_TEXTURE, reg_tex);
        m_assets.push_back(reg_asset.asset.uuid);
        m_textures.push_back(reg_tex.tex.uuid);

        // Prepare texture for usage
        m_rend->prepareAssets(dengMath::vec2<deng_ui32_t>{0, static_cast<deng_ui32_t>(m_assets.size())});
        m_rend->prepareTexture(reg_tex.tex.uuid);
    }


    void OpenGLApplication::__bindFPP() {
        deng::Camera3DBindings bindings = {};
        bindings.mov_u = neko_CreateInputMask(1, NEKO_KEY_D);
        bindings.mov_nu = neko_CreateInputMask(1, NEKO_KEY_A);
        bindings.mov_v = neko_CreateInputMask(1, NEKO_KEY_SPACE);
        bindings.mov_nv = neko_CreateInputMask(1, NEKO_KEY_LEFT_CTRL);
        bindings.mov_w = neko_CreateInputMask(1, NEKO_KEY_S);
        bindings.mov_nw = neko_CreateInputMask(1, NEKO_KEY_W);

        bindings.rot_u = neko_CreateInputMask(1, NEKO_MOUSE_DELTA_NY);
        bindings.rot_nu = neko_CreateInputMask(1, NEKO_MOUSE_DELTA_Y);
        bindings.rot_v = neko_CreateInputMask(1, NEKO_MOUSE_DELTA_NX);
        bindings.rot_nv = neko_CreateInputMask(1, NEKO_MOUSE_DELTA_X);
        bindings.ch_vcp = neko_CreateInputMask(1, NEKO_KEY_ESCAPE);
        m_cam.setBindings(bindings);
    }


    void OpenGLApplication::__bindEditor() {
        deng::Camera3DBindings bindings;
        bindings.mov_w = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP);
        bindings.mov_nw = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_DOWN);
        bindings.ch_vcp = neko_CreateInputMask(1, NEKO_MOUSE_BTN_2);

        m_cam.setBindings(bindings);
    }


    void OpenGLApplication::run() {
        while(m_win.isRunning()) {
            m_win.update();
            m_cam.update();
            m_rend->makeFrame();
        }
    }
}


int main() {
    deng_InitWindowAPI();
    deng::OpenGLApplication app;
    app.run();
    return 0;
}
