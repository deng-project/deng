// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GridGenerator.h - Grid generator class header
// author: Karl-Mihkel Ott

#define GRID_GENERATOR_CPP
#include "deng/GridGenerator.h"

namespace DENG {

    GridGenerator::GridGenerator(
        Entity *_parent, 
        const std::string &_name,
        Renderer &_rend,
        float _width, 
        float _height, 
        float _margin_x, 
        float _margin_y, 
        uint32_t _camera_id,
        const std::string &_framebuffer_id
    ) :
        ScriptableEntity(_parent, _name, ENTITY_TYPE_GRID_MESH),
        m_width(_width),
        m_height(_height),
        m_margin_x(_margin_x),
        m_margin_y(_margin_y),
        m_camera_id(_camera_id),
        m_renderer(_rend),
        m_framebuffer_id(_framebuffer_id) {}


    GridGenerator::~GridGenerator() {
        if (_OnDestroyFunction)
            _OnDestroyFunction(m_script);

        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        mem_man->DeleteUniformMemoryLocation(m_ubo_offset);
        mem_man->DeleteMainMemoryLocation(m_main_offset);
    }

    void GridGenerator::_GenerateVertices(Renderer& _rend) {
        // x: x world
        // y: -z world
        std::vector<TRS::Vector3<float>> vertices;
        const uint32_t xlinec = static_cast<uint32_t>(std::ceil(m_width / m_margin_x));
        const uint32_t ylinec = static_cast<uint32_t>(std::ceil(m_height / m_margin_y));
        vertices.reserve(2 * (xlinec + ylinec));

        // create grid lines from -x to x direction (world coordinates)
        const float begx = -m_width / 2.0f;
        const float begy = -m_height / 2.0f;
        for (uint32_t i = 0; i < ylinec; i++) {
            const float fi = static_cast<float>(i);
            vertices.emplace_back(begx, 0.0f, begy + fi * m_margin_y);
            vertices.emplace_back(-begx, 0.0f, begy + fi * m_margin_y);
        }

        // create grid lines from -y to y direction (world coordinates)
        for (uint32_t i = 0; i < xlinec; i++) {
            const float fi = static_cast<float>(i);
            vertices.emplace_back(begx + fi * m_margin_x, 0.0f, begy);
            vertices.emplace_back(begx + fi * m_margin_x, 0.0f, -begy);
        }

        GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
        m_vert_count = static_cast<uint32_t>(vertices.size());
        m_main_offset = mem_man->RequestMainMemoryLocationP(static_cast<uint32_t>(sizeof(float)), static_cast<uint32_t>(vertices.size() * sizeof(TRS::Vector3<float>)));
        _rend.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<char*>(vertices.data()), static_cast<uint32_t>(vertices.size() * sizeof(TRS::Vector3<float>))), m_main_offset);
    }


    void GridGenerator::_Attach() {
        Registry* reg = Registry::GetInstance();
        Camera3D* cam = (Camera3D*)reg->GetEntityById(m_camera_id);
        m_camera_ubo_offset = cam->GetUboOffset();

        if (_OnAttachFunction)
            _OnAttachFunction(m_script);

        ShaderModule module;
        module.vertex_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 0) in vec3 i_pos;\n"\
            "layout(binding = 0) uniform UniformData {\n"\
            "   vec4 color;\n"\
            "} ubo;\n"\
            "layout(binding = 1) uniform Camera {\n"\
            "   mat4 proj;\n"\
            "   mat4 view;\n"\
            "} camera;\n"\
            ""\
            "layout(location = 0) out vec4 out_color;\n"\
            ""\
            "void main() {\n"\
            "   gl_Position = camera.proj * camera.view * vec4(i_pos, 1.0f);\n"\
            "   out_color = ubo.color;\n"\
            "}\n";
        module.fragment_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 0) in vec4 i_color;\n"\
            "layout(location = 0) out vec4 out_color;\n"\
            ""\
            "void main() {\n"\
            "   out_color = i_color;\n"\
            "}\n";

        GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
        m_ubo_offset = mem_man->RequestUniformMemoryLocationP(m_renderer, static_cast<uint32_t>(sizeof(TRS::Vector4<float>)));
        module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));

        // color uniform
        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 0;
        module.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(TRS::Vector4<float>));
        module.ubo_data_layouts.back().block.offset = m_ubo_offset;
        module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_SHADER;

        // camera uniform
        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 1;
        module.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(ModelCameraUbo));
        module.ubo_data_layouts.back().block.offset = m_camera_ubo_offset;
        module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_SHADER;

        module.enable_blend = true;
        module.enable_depth_testing = true;
        module.enable_texture_mapping = false;
        module.enable_scissor = false;
        module.enable_indexing = false;
        module.prim_mode = PRIMITIVE_MODE_LINES;

        // push the shader module
        m_shader_id = m_renderer.PushShader(module, m_framebuffer_id);
        _GenerateVertices(m_renderer);

        // generate mesh and draw command to submit to renderer
        m_mesh_id = m_renderer.PushMeshReference(MeshReference(), m_framebuffer_id);
        MeshReference& ref = m_renderer.GetMesh(m_mesh_id, m_framebuffer_id);
        ref.name = "__grid__";
        ref.shader_module_id = m_shader_id;
        ref.commands.emplace_back();
        ref.commands.back().draw_count = m_vert_count;
        ref.commands.back().attribute_offsets.push_back(m_main_offset);
    }


    void GridGenerator::_Update() {
        if (_OnUpdateFunction)
            _OnUpdateFunction(m_script);

        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_color), static_cast<uint32_t>(sizeof(TRS::Vector4<float>)), m_ubo_offset);
    }
}
