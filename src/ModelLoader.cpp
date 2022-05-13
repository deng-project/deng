// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.cpp - DAS model loader class implementation
// author: Karl-Mihkel Ott

#define MODEL_LOADER_CPP
#include <ModelLoader.h>

namespace DENG {

    uint32_t ModelLoader::m_model_index = 0;
    uint32_t ModelLoader::m_animation_index = 0;

    ModelLoader::ModelLoader(const std::string &_file_name, Renderer &_rend, uint32_t _base_buffer_offset, uint32_t _base_ubo_offset, uint32_t _camera_offset) : 
        m_parser(_file_name), m_renderer(_rend)
    {
        // reserve enough memory for meshes and animation samplers
        m_parser.Parse();
        MeshLoader::SetMainBufferOffset(_base_buffer_offset);
        MeshLoader::SetUniformBufferOffset(_base_ubo_offset);

        // load each animation in model
        m_animation_samplers.reserve(m_parser.GetAnimationCount());
        for(uint32_t i = 0; i < m_parser.GetAnimationCount(); i++) {
            const Libdas::DasAnimation &ani = m_parser.AccessAnimation(i);
            m_animation_samplers.emplace_back();

            // check if animation name was given
            if(ani.name != "")
                m_animation_samplers.back().first = ani.name;
            else m_animation_samplers.back().first = "Unnamed animation" + std::to_string(m_animation_index++);

            m_animation_samplers.back().second.reserve(ani.channel_count);

            for(uint32_t j = 0; j < ani.channel_count; j++) {
                const Libdas::DasAnimationChannel &channel = m_parser.AccessAnimationChannel(ani.channels[j]);
                m_animation_samplers.back().second.emplace_back(channel, m_parser);
            }
        }

        _AttachBuffersAndTextures();

        // load each scene in the model
        m_scene_loaders.reserve(m_parser.GetSceneCount());
        for(uint32_t i = 0; i < m_parser.GetSceneCount(); i++) {
            const Libdas::DasScene &scene = m_parser.AccessScene(i);
            m_scene_loaders.emplace_back(m_renderer, m_parser, scene, _camera_offset, m_animation_samplers, m_texture_names);
        }

        if(m_parser.GetProperties().model != "")
            m_model_name = m_parser.GetProperties().model;
        else m_model_name += std::to_string(m_model_index++);
    }


    void ModelLoader::_AttachBuffersAndTextures() {
        uint32_t abs_offset = MeshLoader::GetMainBufferOffset();

        // iterate through buffers
        for(uint32_t i = 0; i < m_parser.GetBufferCount(); i++) {
            const Libdas::DasBuffer &buffer = m_parser.AccessBuffer(i);

            // check if the buffer type is texture
            const BufferType texture_mask = LIBDAS_BUFFER_TYPE_TEXTURE_JPEG | LIBDAS_BUFFER_TYPE_TEXTURE_PNG |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_TGA | LIBDAS_BUFFER_TYPE_TEXTURE_TGA |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_BMP | LIBDAS_BUFFER_TYPE_TEXTURE_PPM |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_RAW;

            if(buffer.type & texture_mask) {
                const std::string texture_name = "{" + m_model_name + "}" + "_TEX" + std::to_string(m_texture_names.size());
                m_texture_names.push_back(texture_name);

                // not raw texture data
                if(!(buffer.type & LIBDAS_BUFFER_TYPE_TEXTURE_RAW)) {
                    Libdas::TextureReader reader(buffer.data_ptrs.back());
                    int x, y;
                    size_t len;
                    const char *data = reader.GetRawBuffer(x, y, len);

                    m_renderer.PushTextureFromMemory(texture_name, data, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4);
                } 
                // raw texture
                else {
                    const char *data = buffer.data_ptrs.back().first;
                    uint32_t width = *reinterpret_cast<const uint32_t*>(data);
                    data += sizeof(uint32_t);
                    uint32_t height = *reinterpret_cast<const uint32_t*>(data);
                    data += sizeof(uint32_t);
                    uint8_t bit_depth = *reinterpret_cast<const uint8_t*>(data);
                    data += sizeof(uint8_t);

                    m_renderer.PushTextureFromMemory(texture_name, data, width, height, static_cast<uint32_t>(bit_depth));
                }
            } else {
                m_renderer.UpdateVertexDataBuffer(std::make_pair(buffer.data_ptrs.back().first, static_cast<uint32_t>(buffer.data_ptrs.back().second)), abs_offset);
                abs_offset += buffer.data_ptrs.back().second;
            }
        }

        m_used_main_buffer_memory = static_cast<uint32_t>(abs_offset - MeshLoader::GetMainBufferOffset());
    }

    void ModelLoader::Update() { 
        // update each scene
        for(auto it = m_scene_loaders.begin(); it != m_scene_loaders.end(); it++)
            it->Update();
    }
}
