/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: imgui_sandbox.h - ImGui feature test application header
/// author: Karl-Mihkel Ott

#ifndef __OPENGL_SANDBOX_H
#define __OPENGL_SANDBOX_H


#ifdef __OPENGL_SANDBOX_CPP
    #include <chrono>
    #include <memory>
    #include <vector>
	#include <string>
	#include <array>
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <data/das_loader.h>
    #include <data/tex_loader.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    #include <deng/cross_api/cross_api.h>
    #include <deng/opengl/cfg_vars.h>
    #include <deng/opengl/resources.h>
    #include <deng/opengl/pipelines.h>
    #include <deng/window.h>
    #include <deng/opengl/ubm.h>
    #include <deng/opengl/buffers.h>
    #include <deng/opengl/renderer.h>
#endif

namespace deng {

    class OpenGLApplication {
    private:
        Window m_win;
        Camera3D m_cam;
        opengl::__gl_ConfigVars m_cfg = {};
        Registry m_reg;
        std::vector<deng_Id> m_assets;
        std::vector<deng_Id> m_textures;
        std::unique_ptr<opengl::__gl_Renderer> m_rend;

    private:
        void __bindFPP();
        void __bindEditor();

    public:
        OpenGLApplication();

        /// General run method
        void run();
    };
}

#endif
