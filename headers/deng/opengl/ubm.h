/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.h - OpenGL uniform buffer manager parent class header
/// author: Karl-Mihkel Ott

#ifndef __GL_UBM_H
#define __GL_UBM_H


#ifdef __GL_UBM_CPP
    #include <memory>
    #include <string>
    #include <vector>
    #include <array>
    #include <chrono>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
	#include <common/err_def.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <math/deng_math.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <deng/window.h>
    #include <deng/opengl/resources.h>
    #include <deng/cross_api/cross_api.h>
    #include <deng/cam3d.h>
#endif


namespace deng {
    namespace opengl {


        class __gl_UniformManager : protected __OffsetFinder {
        private:
            __GlobalRegistry &m_reg;
            __gl_Resources &m_resources;
            void (*lglErrorCheck)(const std::string& func_name, const std::string &file, const deng_ui32_t line);

        protected:
            /// Reserve a uniform data memory location for OpenGL asset
            void mapUniformBufferArea(das_Asset &asset);


        public:
            __gl_UniformManager(__GlobalRegistry &reg, std::vector<deng_Id> &assets, __gl_Resources &res, void (*lgl_err_check)(const std::string &func_name, const std::string &file, const deng_ui32_t line));
            

            /// Update uniform transformation data for the frame according to 
            /// camera's view and projection matrix
            void updateUboTransform3D(Camera3D *p_cam);

            
            /// Update asset uniform buffer data
            void updateAssetUboData(das_Asset &asset);

            
            /// Update lighting uniform data
            void updateUboLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);
        };
    }
}

#endif
