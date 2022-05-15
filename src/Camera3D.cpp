// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.cpp - 3D camera base class implementation
// author: Karl-Mihkel Ott

#define CAMERA_3D_CPP
#include <Camera3D.h>

namespace DENG {

    Camera3D::Camera3D(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name) :
        m_renderer(_rend),
        m_window(_win),
        m_config(_conf),
        m_name(_name)
    {
        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();
        m_ubo_offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, static_cast<uint32_t>(sizeof(ModelCameraUbo)));
    }
}
