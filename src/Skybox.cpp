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
        uint32_t _camera_ubo,
        const std::string &_framebuffer,
        const std::string& _right,
        const std::string& _left,
        const std::string &_top,
        const std::string &_bottom,
        const std::string &_back,
        const std::string &_front
    ) : Entity(_parent, m_name_prefix, ENTITY_TYPE_SKYBOX),
        m_renderer(_rend),
        m_camera_offset(_camera_ubo),
        m_framebuffer(_framebuffer)
    {
        m_renderer.PushCubemapFromFiles(m_name_prefix + "TEX" + std::to_string(m_skybox_counter),
                                        _right, _left, _top, _bottom, _back, _front);
    }


    void Skybox::Attach() {
        GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
        uint32_t offset = mem_man->RequestMainMemoryLocationP((uint32_t)sizeof(float), (uint32_t)sizeof(m_skybox_vertices));
        m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_skybox_vertices), (uint32_t)sizeof(m_skybox_counter)), offset);

        uint32_t shader_id = m_renderer.NewMeshReference();
        ShaderModule& module = m_renderer.GetShaderModule(shader_id);

        module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
        module.enable_3d_textures = true;
        module.enable_blend = true;
        module.enable_indexing = false;
        module.vertex_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(std140, binding=0) uniform CameraUbo {\n"\
            "   mat4 projection;\n"\
            "   mat4 view;\n"\
            "} camera;\n"\
            "layout(location = 0) in vec3 in_vert;\n"\
            "layout(location = 0) out vec3 out_uv;\n"\
            "void main() {\n"\
            "   gl_Position = camera.projection * camera.view * vec4(in_vert, 1.0f);\n"\
            "   out_uv = in_vert;\n"\
            "}\n";

        module.fragment_shader_src =
            "#version 450\n"\
            "#extension GL_ARB_separate_shader_objects : enable\n"\
            "layout(location = 0) in vec3 in_uv;\n"\
            "layout(location = 1) out vec4 out_frag;\n"\
            "layout(binding = 1) uniform samplerCube skybox;\n"\
            "void main() {\n"\
            "   out_frag = texture(skybox, in_uv);\n"\
            "}\n";

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
    }
}