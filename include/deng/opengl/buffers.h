/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: buffers.h - OpenGL buffer manager class header
/// author: Karl-Mihkel Ott


#ifndef __GL_BUFFERS_H
#define __GL_BUFFERS_H


#ifdef __GL_BUFFERS_CPP
    #include <string.h>
    #include <vector>
    #include <chrono>
    #include <string>
    #include <array>
    #include <memory>
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/hashmap.h>
	#include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    #include <deng/cross_api/cross_api.h>
    #include <deng/opengl/pipelines.h>
    #include <deng/opengl/resources.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>
    #include <deng/opengl/ubm.h>
#endif

namespace deng {
    namespace opengl {

        class __gl_BufferManager : public __gl_UniformManager {
        private:
            std::vector<deng_Id> &m_assets;
            __ImGuiData *m_p_imgui_data = NULL;
            Registry &m_reg;

            std::shared_ptr<__gl_Pipelines> m_pipelines = {};
            __gl_Resources &m_resources;
            PFNGLERRORCHECK lglErrorCheck;
            std::vector<deng_ugl_t> m_programs;

        public:
            __gl_BufferManager(std::vector<deng_Id> &assets, std::shared_ptr<__gl_Pipelines> pipelines, 
                Registry &reg, PFNGLERRORCHECK lgl_error_check);
            ~__gl_BufferManager();

            /// Check if buffer reallocation is needed for assets and gui elements
            deng_bool_t reallocCheck();

            /// Allocate ui_cap + asset_cap amount of memory for the buffers
            void allocateBufferMemory();

            /// Copy all asset data between given bounds to buffer
            /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
            void cpyAssetToBuffer(const deng_Id id);


            /// Copy ImGui vertex and indices data to buffer
            /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
            void cpyUIDataToBuffer(deng_bool_t no_offset_calc);


            /// Enable and set vertex attribute pointers according to the specified asset
            void bindAssetDataBufferRegion(const das_Asset &asset);


            /// Disable vertex attributes for specified asset
            void unbindAssetDataBufferRegion(const das_Asset &asset);

        // Setter and getter methods
        public:
            __gl_Resources getResources();
            void setUIDataPtr(__ImGuiData *p_gui);
        };
    }
}

#endif
