// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: PointViewer.cpp - Point viewer class implementation
// author: Karl-Mihkel Ott

#define POINT_VIEWER_CPP
#include <PointViewer.h>

namespace DENG {

    PointViewer::PointViewer(Renderer &_rend, float _width, uint32_t _main_buffer_offset, uint32_t _ubo_offset) :
        m_renderer(_rend), 
        m_main_buffer_offset(_main_buffer_offset),
        m_ubo_offset(_ubo_offset),
        m_point_square_width(_width)
    {
        _CalculateVertices();
        m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_point_vertices), static_cast<uint32_t>(sizeof(Libdas::Point3D<float>[4]))), m_main_buffer_offset);
    }


    void PointViewer::_CalculateVertices() {
        m_point_vertices[0] = Libdas::Point3D<float>{ 
            -m_point_square_width, 
            m_point_square_width, 
            0.0f 
        };

        m_point_vertices[1] = Libdas::Point3D<float> {
            m_point_square_width,
            m_point_square_width,
            0.0f
        };

        m_point_vertices[2] = Libdas::Point3D<float> {
            m_point_square_width,
            -m_point_square_width,
            0.0f
        };

        m_point_vertices[3] = Libdas::Point3D<float> {
            -m_point_square_width,
            -m_point_square_width,
            0.0f
        };
    }


    void PointViewer::Update(Libdas::Matrix4<float> &_transform, float _width) {
        if(_width != 0.0f) {
            m_point_square_width = _width;
            _CalculateVertices();
        }

        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&_transform), static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)), m_ubo_offset);
    }
}
