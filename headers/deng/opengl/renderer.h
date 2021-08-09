/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.h - OpenGL renderer class header
/// author: Karl-Mihkel Ott


#ifndef __GL_RENDERER_H
#define __GL_RENDERER_H


#ifdef __GL_RENDERER_CPP
    #include <vector>
    #include <chrono>
    #include <array>
    #include <memory>
    #include <string>
    #include <glad/glad.h>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/uuid.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <data/das_runtime.h>
    
    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    #include <deng/cam3d.h>

    #include <deng/cross_api/cross_api.h>
    #include <deng/opengl/cfg_vars.h>
    #include <deng/opengl/pipelines.h>
    #include <deng/opengl/resources.h>
    #include <deng/window.h>
    #include <deng/opengl/ubm.h>
    #include <deng/opengl/buffers.h>
#endif

namespace deng {
    namespace opengl {

        class __gl_Renderer {
        private:
            __gl_ConfigVars m_cfg_vars;
            deng::__GlobalRegistry &m_reg;
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_textures;

            deng_ui32_t m_vert_buffer;

            // Helper objects
            std::shared_ptr<__gl_Pipelines> m_pipelines;
            std::unique_ptr<__gl_BufferManager> m_buf_manager;
            
        private:
            /// OpenGL error checking function
            static void glErrorCheck(const std::string &func_name);

        public:
            __gl_Renderer(__gl_ConfigVars &cfg, deng::__GlobalRegistry &reg, std::vector<deng_Id> &assets,
                std::vector<deng_Id> &textures);


            /// Update all currently available light sources
            void setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);


            /// Prepare an asset for OpenGL usage
            void prepareAssets(const dengMath::vec2<deng_ui32_t> &bounds);

            /// Prepare texture for OpenGL usage
            void prepareTexture(const deng_Id id);
            

            // Main frame updating function
            void makeFrame();

        // Setter and getter methods
        public:
            void setUIDataPtr(__ImGuiData *p_data);
            const deng_bool_t isInit();
        };
    }
}

#endif
