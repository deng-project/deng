/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: OpenGLBuffers.h - OpenGL buffer manager class header
/// author: Karl-Mihkel Ott


#ifndef OPENGL_BUFFERS_H
#define OPENGL_BUFFERS_H


#ifdef OPENGL_BUFFERS_CPP
    #include <string.h>
    #include <vector>
    #include <chrono>
    #include <string>
    #include <array>
    #include <memory>
    #include <vulkan/vulkan.h>
    #include <base_types.h>
    #include <common.h>
    #include <hashmap.h>
	#include <err_def.h>
    #include <assets.h>

    #include <deng_math.h>
    #include <registry.h>
    #include <imgui_entity.h>
    #include <cross_api.h>
    #include <pipelines.h>
    #include <resources.h>
    #include <window.h>
    #include <cam3d.h>
    #include <ubm.h>
#endif

namespace DENG {
    namespace OpenGL {

        class BufferManager : public __gl_UniformManager {
        private:
            std::vector<deng_Id> &m_assets;
            ImGuiData *m_p_imgui_data = NULL;
            Registry &m_reg;

            std::shared_ptr<__gl_Pipelines> m_pipelines = {};
            Resources &m_resources;
            PFNGLERRORCHECK lglErrorCheck;
            std::vector<uint32_t> m_programs;

        public:
            BufferManager(std::vector<deng_Id> &assets, std::shared_ptr<__gl_Pipelines> pipelines, 
                Registry &reg, PFNGLERRORCHECK lgl_error_check);
            ~BufferManager();

            /// Check if buffer reallocation is needed for assets and gui elements
            bool ReallocCheck();

            /// Allocate ui_cap + asset_cap amount of memory for the buffers
            void AllocateBufferMemory();

            /// Copy all asset data between given bounds to buffer
            /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
            void CpyAssetToBuffer(const deng_Id id);


            /// Copy ImGui vertex and indices data to buffer
            /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
            void CpyUIDataToBuffer(bool no_offset_calc);


            /// Enable and set vertex attribute pointers according to the specified asset
            void BindAssetDataBufferRegion(const das_Asset &asset);


            /// Disable vertex attributes for specified asset
            void UnbindAssetDataBufferRegion(const das_Asset &asset);

        // Setter and getter methods
        public:
            Resources GetResources();
            void SetUIDataPtr(ImGuiData *p_gui);
        };
    }
}

#endif
