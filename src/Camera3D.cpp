// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.cpp - 3D camera base class implementation
// author: Karl-Mihkel Ott

#define CAMERA_3D_CPP
#include "deng/Camera3D.h"

namespace DENG {

    Camera3D::Camera3D(Entity *_parent, const std::string &_name, EntityType _type, Renderer &_rend) :
        ScriptableEntity(_parent, _name, _type),
        m_renderer(_rend) {}


    Camera3D::~Camera3D() {
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        mem_man->DeleteUniformMemoryLocation(m_ubo_offset);
    }
}
