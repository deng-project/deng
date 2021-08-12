/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dengbox.h - DENG sandbox application header
/// author: Karl-Mihkel Ott

#ifndef __DENGBOX_H
#define __DENGBOX_H


#ifdef __DENGBOX_CPP
    #include <vector>
	#include <array>
    #include <thread>
    #include <chrono>
    #include <mutex>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <common/common.h>
    #include <common/uuid.h>
    #include <data/assets.h>
    #include <data/das_loader.h>
    #include <data/tex_loader.h>

    #include <math/deng_math.h>
    #include <deng/window.h>

    #include <deng/registry/registry.h>
    #include <deng/lighting/light_man.h>
    #include <deng/cam3d.h>
    #include <deng/window.h>
    #include <deng/renderer/renderer.h>
#endif

namespace Sandbox {
    
    /// Vulkan sandbox application class
    class DengBox {
    private:
        deng::Window m_win;
        deng::Camera3D m_cam;
        deng::Registry m_reg;
        dengMath::Transformer3D m_transformer;
        deng::LightManager m_light_man;
        std::unique_ptr<deng::Renderer> m_rend;
        std::vector<das_Asset> m_assets;
        std::vector<das_Texture> m_textures;

    private: 
        /// Create first person camera control bindings
        void __bindFPP();


        /// Create editor camera control bindings
        void __bindEditor();


        /// Load test assets into the register
        void __loadAssets(const std::vector<const char*> &files);

        
        /// Load test textures into the register 
        void __loadTextures(const std::vector<const char*> &files);


        /// Create new light sources
        void __mkLightSources();

    public:
        DengBox();
        void run();
        void setup();
    };
}

#endif
