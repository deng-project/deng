/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dengbox.h - DENG sandbox application implementation
/// author: Karl-Mihkel Ott


#define __DENGBOX_CPP
#include <../src/sandbox/dengbox/dengbox.h>


namespace Sandbox {

    DengBox::DengBox() : 
        m_win(1200, 1000, DENG_WINDOW_HINT_API_VULKAN | DENG_WINDOW_HINT_FIXED_SIZE, "DENG game"),
        m_cam(DENG_CAMERA_TYPE_FPP, static_cast<deng_vec_t>(dengMath::conversion::degToRad(65.0)),
              {0.1f, -25.0f}, {0.7f, 0.7f, 0.7f}, {0.3f, 0.3f}, &m_win),
        m_light_man(m_reg) {

        // Setup code that will be done in Lua when the scripting layer is added
        switch(m_cam.getType()) {
        case DENG_CAMERA_TYPE_EDITOR:
            __bindEditor();
            break;

        case DENG_CAMERA_TYPE_FPP:
            __bindFPP();
            break;

        default:
            break;
        }

        __loadTextures ({ "textures/viking_room.tga" });
        __loadAssets ({ "assets/viking.das" });

        // Create some demo light sources
        __mkLightSources();

        m_assets.back().tex_uuid = m_textures[0].uuid;

        #ifdef __DEBUG
            m_rend = std::make_unique<deng::Renderer>(DENG_RENDERER_HINT_ENABLE_API_DEBUGGING | 
                                                      DENG_RENDERER_HINT_MIPMAP_ENABLE | DENG_RENDERER_HINT_MSAA_4,
                                                      dengMath::vec4<deng_vec_t>{ 0.0f, 0.0f, 0.0f, 1.0f });
        #else 
            m_rend = std::make_unique<deng::Renderer>(DENG_RENDERER_HINT_MIPMAP_ENABLE | DENG_RENDERER_HINT_MSAA_4,
                                                      dengMath::vec4<deng_vec_t>{ 0.0f, 0.0f, 0.0f, 1.0f });
        #endif

        // Submit all assets and textures to renderer
        for(size_t i = 0; i < m_assets.size(); i++)
            m_rend->submitAsset(m_assets[i]);

        for(size_t i = 0; i < m_textures.size(); i++)
            m_rend->submitTexture(m_textures[i]);
    }


    /// Create first person camera control bindings
    void DengBox::__bindFPP() {
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

        m_cam.setBindings(bindings);
    }


    /// Create editor camera control bindings
    void DengBox::__bindEditor() {
        deng::Camera3DBindings bindings;
        bindings.mov_w = deng_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP);
        bindings.mov_nw = deng_CreateInputMask(1, NEKO_MOUSE_SCROLL_DOWN);
        bindings.ch_vcp = deng_CreateInputMask(1, NEKO_MOUSE_BTN_2);

        m_cam.setBindings(bindings);
    }


    /// Load test assets into the register
    void DengBox::__loadAssets(const std::vector<const char*> &files) {
        for(size_t i = 0; i < files.size(); i++) {
            das_Asset asset = {};
            das_LoadAsset(&asset, DAS_ASSET_MODE_UNDEFINED,
                {0.0f, 0.0f, 1.0f}, false, NULL, (char*) files[i]);

            // Push asset to the queue
            m_assets.push_back(asset);
        }
    }

    
    /// Load test textures into the register 
    void DengBox::__loadTextures(const std::vector<const char*> &files) {
        for(size_t i = 0; i < files.size(); i++) {
            das_Texture tex = {};
            das_LoadTexture(&tex, files[i]);
            
            m_textures.push_back(tex);
        }
    }


    /// Create new light sources
    void DengBox::__mkLightSources() {
        m_light_man.newPtLightSrc(5.0f, dengMath::vec3<deng_vec_t>{0.0f, -0.5f, 0.0f});
        m_light_man.newPtLightSrc(10.0f, dengMath::vec3<deng_vec_t>{0.0f, 0.0f, -1.0f});
    }


    void DengBox::setup() {
        m_rend->setup(m_cam, m_win);
    }


    void DengBox::run() {
        m_rend->run();
    }
}

int main(void) {
    deng_InitWindowAPI();
    {
        Sandbox::DengBox deng_app;
        deng_app.setup();
        deng_app.run();
    }
    deng_DeinitWindowAPI();
    return 0;
}
