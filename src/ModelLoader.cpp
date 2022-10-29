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
        const Libdas::DasModel &_model, 
        Renderer &_rend, 
        uint32_t _camera_id,
        const std::string &_framebuffer
    ) : 
        ScriptableEntity(_parent, "", ENTITY_TYPE_MODEL),
        m_model(_model), 
        m_renderer(_rend),
        m_camera_id(_camera_id),
        m_framebuffer_id(_framebuffer)
    {
        std::string name = "Unnamed model";
        if(m_model.props.model != "")
            name = m_model.props.model;
        else name += std::to_string(m_model_index++);
        
        SetName(name);
    }


    ModelLoader::ModelLoader(ModelLoader &&_ld) noexcept :
        ScriptableEntity(std::move(_ld)),
        m_model(std::move(_ld.m_model)),
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
                it->_SetModel(m_model);
            }
        }
    }


    ModelLoader::~ModelLoader() {
        GPUMemoryManager *mem_man = GPUMemoryManager::GetInstance();
        for(uint32_t offset : m_buffer_offsets)
            mem_man->DeleteMainMemoryLocation(offset);
    }


    void ModelLoader::_AttachBuffersAndTextures() {
        m_buffer_offsets.reserve(m_model.buffers.size());

        // iterate through buffers
        for(size_t i = 0; i < m_model.buffers.size(); i++) {
            const Libdas::DasBuffer &buffer = m_model.buffers[i];

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
                m_buffer_offsets.push_back(mem_manager->RequestMainMemoryLocationP(4, buffer.data_len));

                uint32_t rel_offset = 0;
                for (auto ptr_it = buffer.data_ptrs.begin(); ptr_it != buffer.data_ptrs.end(); ptr_it++) {
                    m_renderer.UpdateVertexDataBuffer(std::make_pair(ptr_it->first, static_cast<uint32_t>(ptr_it->second)), m_buffer_offsets[i] + rel_offset);
                    rel_offset += ptr_it->second;
                }
            }
        }
    }


    void ModelLoader::Attach() {
        _AttachBuffersAndTextures();

        // load each animation in model
        m_animations.reserve(m_model.animations.size());
        for (size_t i = 0; i < m_model.animations.size(); i++) {
            const Libdas::DasAnimation& ani = m_model.animations[i];
            m_animations.emplace_back();

            // check if animation name was given
            if (ani.name != "")
                m_animations.back().name = ani.name;
            else m_animations.back().name = "Unnamed animation" + std::to_string(m_animation_index++);

            m_animations.back().samplers.reserve(ani.channel_count);

            for (uint32_t j = 0; j < ani.channel_count; j++) {
                const std::string name = "{" + m_animations.back().name + "}_sampler" + std::to_string(j);
                const Libdas::DasAnimationChannel& channel = m_model.channels[ani.channels[j]];
                m_animations.back().samplers.emplace_back(this, name, channel);
            }
        }


        Registry* reg = Registry::GetInstance();
        const uint32_t offset = ((Camera3D*)reg->GetEntityById(m_camera_id))->GetUboOffset();

        // load each scene in the model
        m_scene_loaders.reserve(m_model.scenes.size());
        for (auto it = m_model.scenes.begin(); it != m_model.scenes.end(); it++) {
            m_scene_loaders.emplace_back(this, m_renderer, m_model, *it, m_buffer_offsets, offset, m_animations, m_framebuffer_id);
        }

        SetAttachedBit(true);
    }


    void ModelLoader::Update() { 
        // update each scene
        for(auto it = m_scene_loaders.begin(); it != m_scene_loaders.end(); it++)
            it->Update();
    }
}
