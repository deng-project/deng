// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Renderer.cpp - Base renderer class implementation
// author: Karl-Mihkel Ott

#define RENDERER_CPP
#include <Renderer.h>

namespace DENG {
    RendererBackend Renderer::m_backend = RENDERER_BACKEND_UNKNOWN;
}
