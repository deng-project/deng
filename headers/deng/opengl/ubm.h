/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ubm.h - OpenGL uniform buffer manager parent class header
/// author: Karl-Mihkel Ott

#ifndef __GL_UBM_H
#define __GL_UBM_H


#ifdef __GL_UBM_CPP
    #include <string.h>
    #include <memory>
    #include <string>
    #include <vector>
    #include <array>
    #include <chrono>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
	#include <common/err_def.h>
    #include <common/hashmap.h>
    #include <common/common.h>
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
            Registry &m_reg;
            __gl_Resources m_resources;
            deng_ui64_t m_global_ubo_chunk_size;
            deng_ui64_t m_const_offset;
            deng_ui64_t m_min_align;
            PFNGLERRORCHECK lglErrorCheck;

        private:
            void __bindUniformBlocks(const std::vector<deng_ugl_t> &programs);
            void __bindUniformBlockToBindingPoint(const std::string &block_name, const deng_ugl_t bpt, const deng_ugl_t program);

        protected:
            /// Reserve a uniform data memory location for OpenGL asset
            void mapUniformBufferArea(das_Asset &asset);

            __gl_Resources &getResources();


        public:
            __gl_UniformManager(Registry &reg, std::vector<deng_Id> &assets, PFNGLERRORCHECK lgl_error_check,
                                const std::vector<deng_ugl_t> &programs);
            ~__gl_UniformManager();
            
            /// Update uniform transformation data for the frame according to 
            /// camera's view and projection matrix
            void updateUboTransform3D(Camera3D *p_cam);

            
            /// Update asset uniform buffer data
            void updateAssetUboData(const das_Asset &asset);

            
            /// Update lighting uniform data
            void updateUboLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);


            /// Bind uniform buffer range for specified asset
            void bindAssetUboBufferRange(const das_Asset &asset);
        };
    }
}

#endif
