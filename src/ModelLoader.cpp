// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.cpp - DAS model loader class implementation
// author: Karl-Mihkel Ott

#define MODEL_LOADER_CPP
#include "deng/ModelLoader.h"

namespace DENG {

    uint32_t ModelLoader::m_model_index = 0;
    uint32_t ModelLoader::m_animation_index = 0;

    ModelLoader::ModelLoader(
        Entity *_parent,
        const std::string &_file_name, 
        Renderer &_rend, 
        uint32_t _camera_id,
        const std::string &_framebuffer
    ) : 
        ScriptableEntity(_parent, "", ENTITY_TYPE_MODEL),
        m_parser(_file_name), 
        m_renderer(_rend),
        m_camera_id(_camera_id),
        m_framebuffer_id(_framebuffer)
    {
        m_parser.Parse();

        std::string name = "Unnamed model";
        if(m_parser.GetProperties().model != "")
            name = m_parser.GetProperties().model;
        else name += std::to_string(m_model_index++);
        
        SetName(name);
    }


    ModelLoader::ModelLoader(ModelLoader &&_ld) noexcept :
        ScriptableEntity(std::move(_ld)),
        m_parser(std::move(_ld.m_parser)),
        m_renderer(_ld.m_renderer),
        m_animations(std::move(_ld.m_animations)),
        m_scene_loaders(std::move(_ld.m_scene_loaders)),
        m_texture_names(std::move(_ld.m_texture_names)),
        m_buffer_offsets(std::move(_ld.m_buffer_offsets)),
        m_camera_id(_ld.m_camera_id),
        m_framebuffer_id(_ld.m_framebuffer_id) 
    {
        if (GetAttachedBit()) {
            for (auto it = m_scene_loaders.begin(); it != m_scene_loaders.end(); it++) {
                it->_SetParser(m_parser);
            }
        }
    }


    ModelLoader::~ModelLoader() {
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        for(uint32_t offset : m_buffer_offsets)
            mem_man->DeleteMainMemoryLocation(offset);
    }


    void ModelLoader::_AttachBuffersAndTextures() {
        m_buffer_offsets.reserve(m_parser.GetBufferCount());

        // iterate through buffers
        for(uint32_t i = 0; i < m_parser.GetBufferCount(); i++) {
            const Libdas::DasBuffer &buffer = m_parser.AccessBuffer(i);

            // check if the buffer type is texture
            const BufferType texture_mask = LIBDAS_BUFFER_TYPE_TEXTURE_JPEG | LIBDAS_BUFFER_TYPE_TEXTURE_PNG |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_TGA | LIBDAS_BUFFER_TYPE_TEXTURE_TGA |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_BMP | LIBDAS_BUFFER_TYPE_TEXTURE_PPM |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_RAW;

            if(buffer.type & texture_mask) {
                const std::string texture_name = "{" + GetName() + "}" + "_TEX" + std::to_string(m_texture_names.size());
                m_texture_names.push_back(texture_name);

                // not raw texture data
                if(!(buffer.type & LIBDAS_BUFFER_TYPE_TEXTURE_RAW)) {
                    Libdas::TextureReader reader(buffer.data_ptrs.back());
                    int x, y;
                    size_t len;
                    const char *data = reader.GetRawBuffer(x, y, len);

                    m_renderer.PushTextureFromMemory(texture_name, data, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4);
                }  else { // raw texture
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
                m_buffer_offsets.push_back(mem_manager->RequestMainMemoryLocationP(4, static_cast<uint32_t>(buffer.data_ptrs.back().second)));
                m_renderer.UpdateVertexDataBuffer(std::make_pair(buffer.data_ptrs.back().first, static_cast<uint32_t>(buffer.data_ptrs.back().second)), m_buffer_offsets[i]);
            }
        }
    }


    void ModelLoader::Attach() {
        _AttachBuffersAndTextures();

        // load each animation in model
        m_animations.reserve(m_parser.GetAnimationCount());
        for (uint32_t i = 0; i < m_parser.GetAnimationCount(); i++) {
            const Libdas::DasAnimation& ani = m_parser.AccessAnimation(i);
            m_animations.emplace_back();

            // check if animation name was given
            if (ani.name != "")
                m_animations.back().name = ani.name;
            else m_animations.back().name = "Unnamed animation" + std::to_string(m_animation_index++);

            m_animations.back().samplers.reserve(ani.channel_count);

            for (uint32_t j = 0; j < ani.channel_count; j++) {
                const std::string name = "{" + m_animations.back().name + "}_sampler" + std::to_string(j);
                const Libdas::DasAnimationChannel& channel = m_parser.AccessAnimationChannel(ani.channels[j]);
                m_animations.back().samplers.emplace_back(this, name, channel, m_parser);
            }
        }


        Registry* reg = Registry::GetInstance();
        const uint32_t offset = ((Camera3D*)reg->GetEntityById(m_camera_id))->GetUboOffset();

        // load each scene in the model
        m_scene_loaders.reserve(m_parser.GetSceneCount());
        for (uint32_t i = 0; i < m_parser.GetSceneCount(); i++) {
            const Libdas::DasScene& scene = m_parser.AccessScene(i);
            m_scene_loaders.emplace_back(this, m_renderer, m_parser, scene, m_buffer_offsets, offset, m_animations, m_framebuffer_id);
        }

        SetAttachedBit(true);
    }


    void ModelLoader::Update() { 
        // update each scene
        for(auto it = m_scene_loaders.begin(); it != m_scene_loaders.end(); it++)
            it->Update();
    }
}
