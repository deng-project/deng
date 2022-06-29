// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.cpp - DAS model loader class implementation
// author: Karl-Mihkel Ott

#define MODEL_LOADER_CPP
#include <ModelLoader.h>

namespace DENG {

    uint32_t ModelLoader::m_model_index = 0;
    uint32_t ModelLoader::m_animation_index = 0;

    ModelLoader::ModelLoader(
        const std::string &_file_name, 
        Renderer &_rend, 
        uint32_t _camera_offset,
        const std::string &_framebuffer
    ) : 
        m_parser(_file_name), 
        m_renderer(_rend),
        m_framebuffer_id(_framebuffer)
    {
        // reserve enough memory for meshes and animation samplers
        m_parser.Parse();

        // load each animation in model
        m_animations.reserve(m_parser.GetAnimationCount());
        for(uint32_t i = 0; i < m_parser.GetAnimationCount(); i++) {
            const Libdas::DasAnimation &ani = m_parser.AccessAnimation(i);
            m_animations.emplace_back();

            // check if animation name was given
            if(ani.name != "")
                m_animations.back().name = ani.name;
            else m_animations.back().name = "Unnamed animation" + std::to_string(m_animation_index++);

            m_animations.back().samplers.reserve(ani.channel_count);

            for(uint32_t j = 0; j < ani.channel_count; j++) {
                const Libdas::DasAnimationChannel &channel = m_parser.AccessAnimationChannel(ani.channels[j]);
                m_animations.back().samplers.emplace_back(channel, m_parser);
            }
        }

        _AttachBuffersAndTextures();

        // load each scene in the model
        m_scene_loaders.reserve(m_parser.GetSceneCount());
        for(uint32_t i = 0; i < m_parser.GetSceneCount(); i++) {
            const Libdas::DasScene &scene = m_parser.AccessScene(i);
            m_scene_loaders.emplace_back(m_renderer, m_parser, scene, m_buffer_offsets, _camera_offset, m_animations, m_framebuffer_id);
        }

        if(m_parser.GetProperties().model != "")
            m_model_name = m_parser.GetProperties().model;
        else m_model_name += std::to_string(m_model_index++);
    }


    ModelLoader::ModelLoader(ModelLoader &&_ld) noexcept :
        m_parser(std::move(_ld.m_parser)),
        m_renderer(_ld.m_renderer),
        m_animations(std::move(_ld.m_animations)),
        m_scene_loaders(std::move(_ld.m_scene_loaders)),
        m_model_name(std::move(_ld.m_model_name)),
        m_texture_names(std::move(_ld.m_texture_names)),
        m_buffer_offsets(std::move(_ld.m_buffer_offsets)),
        m_framebuffer_id(_ld.m_framebuffer_id) {}


    ModelLoader::~ModelLoader() {
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        for(uint32_t offset : m_buffer_offsets)
            mem_man->DeleteMainMemoryLocation(offset);
    }


    void ModelLoader::_AttachBuffersAndTextures() {
        m_buffer_offsets.resize(m_parser.GetBufferCount());
        std::fill(m_buffer_offsets.begin(), m_buffer_offsets.end(), UINT32_MAX);

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
                GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();
                m_buffer_offsets[i] = mem_manager->RequestMainMemoryLocationP(1, static_cast<uint32_t>(buffer.data_ptrs.back().second));
                m_renderer.UpdateVertexDataBuffer(std::make_pair(buffer.data_ptrs.back().first, static_cast<uint32_t>(buffer.data_ptrs.back().second)), m_buffer_offsets[i]);
            }
        }
    }

    void ModelLoader::Update() { 
        // update each scene
        for(auto it = m_scene_loaders.begin(); it != m_scene_loaders.end(); it++)
            it->Update();
    }
}
