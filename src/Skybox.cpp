// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Skybox.cpp - Skybox generation class implementation
// author: Karl-Mihkel Ott

#define SKYBOX_CPP
#include "deng/Skybox.h"

namespace DENG {
	const float Skybox::m_skybox_vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
	};

    const std::string Skybox::m_name_prefix = "__skybox__";
    uint32_t Skybox::m_skybox_counter = 0;

    Skybox::Skybox(
        Entity* _parent,
        Renderer& _rend,
        uint32_t _camera_id,
        const std::string& _right,
        const std::string& _left,
        const std::string &_top,
        const std::string &_bottom,
        const std::string &_back,
        const std::string &_front,
        const std::string& _framebuffer
    ) : Entity(_parent, m_name_prefix, ENTITY_TYPE_SKYBOX),
        m_renderer(_rend),
        m_framebuffer(_framebuffer)
    {
        m_renderer.PushCubemapFromFiles(m_name_prefix + "TEX" + std::to_string(m_skybox_counter),
                                        _right, _left, _top, _bottom, _back, _front);

        Registry* reg = Registry::GetInstance();
        Entity *ent = reg->GetEntityById(_camera_id);
        m_camera_offset = ((Camera3D*)ent)->GetUboOffset();
    }


    void Skybox::Attach() {
        GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
        uint32_t offset = mem_man->RequestMainMemoryLocationP((uint32_t)sizeof(float), (uint32_t)sizeof(m_skybox_vertices));
        m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_skybox_vertices), (uint32_t)sizeof(m_skybox_vertices)), offset);

        ShaderModule module;
        module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
        module.enable_3d_textures = true;
        module.enable_depth_testing = true;
        module.enable_blend = true;
        module.enable_indexing = false;
        module.vertex_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(std140, binding=0) uniform CameraUbo {\n"\
            "   mat4 projection;\n"\
            "   mat4 view;\n"\
            "} camera;\n"\
            "layout(std140, binding=1) uniform ScaleUbo {\n"\
            "   vec4 scale;\n"\
            "} scale;\n"\
            "layout(location = 0) in vec3 in_vert;\n"\
            "layout(location = 1) out vec3 out_uv;\n"\
            "void main() {\n"\
            "   vec4 pos = vec4(in_vert, 1.0f) * scale.scale;\n"\
            "   gl_Position = camera.projection * camera.view * pos;\n"\
            "   out_uv = in_vert;\n"\
            "}\n";

        module.fragment_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 1) in vec3 in_uv;\n"\
            "layout(location = 0) out vec4 out_frag;\n"\
            "layout(binding = 2) uniform samplerCube skybox;\n"\
            "void main() {\n"\
            "   out_frag = texture(skybox, in_uv);\n"\
            "}\n";
        
        module.ubo_data_layouts.reserve(3);
        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 0;
        module.ubo_data_layouts.back().block.offset = m_camera_offset;
        module.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(ModelCameraUbo));
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_SHADER;

        m_ubo_offset = mem_man->RequestUniformMemoryLocationF(m_renderer, sizeof(TRS::Vector4<float>));
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), (uint32_t)sizeof(TRS::Vector4<float>), m_ubo_offset);

        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 1;
        module.ubo_data_layouts.back().block.offset = m_ubo_offset;
        module.ubo_data_layouts.back().block.size = (uint32_t)sizeof(TRS::Vector4<float>);
        module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        module.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_SHADER;

        module.ubo_data_layouts.emplace_back();
        module.ubo_data_layouts.back().block.binding = 2;
        module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
        module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_IMAGE_SAMPLER;
        module.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_SHADER;
        uint32_t shader_id = m_renderer.PushShader(module, m_framebuffer);

        MeshReference mesh;
        mesh.shader_module_id = shader_id;
        mesh.name = m_name_prefix + std::to_string(m_skybox_counter);
        DrawCommand cmd;
        cmd.attribute_offsets.push_back(offset);
        cmd.texture_names.push_back(m_name_prefix + "TEX" + std::to_string(m_skybox_counter));
        cmd.draw_count = 36;
        mesh.commands.emplace_back(std::move(cmd));
        m_renderer.PushMeshReference(mesh, m_framebuffer);
        m_skybox_counter++;

        SetAttachedBit(true);
    }


    void Skybox::Update() {
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), (uint32_t)sizeof(TRS::Vector4<float>), m_ubo_offset);
    }
}