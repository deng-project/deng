// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: PointViewer.cpp - Point viewer class implementation
// author: Karl-Mihkel Ott

#define POINT_VIEWER_CPP
#include <PointViewer.h>

namespace DENG {

    uint32_t PointViewer::m_shader_id = UINT32_MAX;
    uint32_t PointViewer::m_point_nr = 0;

    PointViewer::PointViewer(Renderer &_rend, float _width, uint32_t _main_buffer_offset, uint32_t _ubo_offset) :
        m_renderer(_rend), 
        m_main_buffer_offset(_main_buffer_offset),
        m_ubo_offset(_ubo_offset),
        m_point_square_width(_width)
    {
        _CalculateVertices();
        m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_point_vertices), static_cast<uint32_t>(sizeof(Libdas::Point3D<float>[4]))), m_main_buffer_offset);
        m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_point_indices), static_cast<uint32_t>(sizeof(uint32_t[6]))), m_main_buffer_offset + static_cast<uint32_t>(sizeof(Libdas::Point3D<float>[4])));

        if(m_shader_id == UINT32_MAX)
            _GenerateShaderModule(m_renderer, _main_buffer_offset, _ubo_offset);

        // generate a mesh reference
        m_renderer.PushMeshReference(MeshReference());
        MeshReference &ref = m_renderer.GetMeshes().back();
        ref.name = "__point" + std::to_string(m_point_nr++) + "__";
        ref.shader_module_id = m_shader_id;
        ref.commands.emplace_back();
        ref.commands.back().indices_offset = m_main_buffer_offset + static_cast<uint32_t>(sizeof(m_point_vertices));
        ref.commands.back().indices_count = static_cast<uint32_t>(sizeof(m_point_vertices) / sizeof(Libdas::Point3D<float>));
        ref.commands.back().attribute_offsets.push_back(m_main_buffer_offset);
        ref.ubo_blocks.emplace_back();
        ref.ubo_blocks.back().binding = 0;
        ref.ubo_blocks.back().size = sizeof(Libdas::Matrix4<float>);
        ref.ubo_blocks.back().offset = m_ubo_offset;
    }


    void PointViewer::_GenerateShaderModule(Renderer &_rend, uint32_t _main_offset, uint32_t _ubo_offset) {
        ShaderModule module;
        module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attribute_strides.push_back(sizeof(Libdas::Vector3<float>));
        module.ubo_data_layouts.push_back({
            { 0, sizeof(Libdas::Matrix4<float>), _ubo_offset },
            UNIFORM_DATA_TYPE_BUFFER,
            SHADER_STAGE_VERTEX,
            UNIFORM_USAGE_PER_MESH
        });
        module.vertex_shader_src = 
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 0) in vec3 pos;"
            "layout(std140, set = 0, binding = 0) uniform Transform {\n"\
            "   mat4 mat;\n"\
            "} tr;\n"\
            "void main() {\n"\
            "   gl_Position = tr.mat * vec4(pos, 1.0f);\n"\
            "}\n";

        module.fragment_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 0) out vec4 color;\n"\
            "void main() {\n"\
            "   color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"\
            "}\n";

        m_shader_id = _rend.PushShader(module);
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
            m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_point_vertices), static_cast<uint32_t>(sizeof(Libdas::Point3D<float>[4]))), m_main_buffer_offset);
        }

        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&_transform), static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)), m_ubo_offset);
    }
}
