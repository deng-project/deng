// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.cpp - DAS model loader class implementation
// author: Karl-Mihkel Ott

// **** EXPERIMENTAL **** //
#define MODEL_LOADER_CPP
#include <ModelLoader.h>

namespace DENG {

    ModelLoader::ModelLoader(const std::string &_file_name, Renderer &_rend, uint32_t _base_buffer_offset, uint32_t _base_ubo_offset) : 
        m_parser(_file_name), m_renderer(_rend), m_base_buffer_offset(_base_buffer_offset), m_base_ubo_offset(_base_ubo_offset)
    {
        // reserve enough memory for meshes and animation samplers
        m_mesh_loaders.reserve(m_parser.GetMeshCount());
        m_animations.reserve(m_parser.GetAnimationCount());
    }


    void ModelLoader::_AttachBuffersAndTextures() {
        size_t abs_offset = m_base_buffer_offset;

        // iterate through buffers
        for(uint32_t i = 0; i < m_parser.GetBufferCount(); i++) {
            const Libdas::DasBuffer &buffer = m_parser.AccessBuffer(i);

            // check if the buffer type is texture
            const BufferType texture_mask = LIBDAS_BUFFER_TYPE_TEXTURE_JPEG | LIBDAS_BUFFER_TYPE_TEXTURE_PNG |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_TGA | LIBDAS_BUFFER_TYPE_TEXTURE_TGA |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_BMP | LIBDAS_BUFFER_TYPE_TEXTURE_PPM |
                                            LIBDAS_BUFFER_TYPE_TEXTURE_RAW;

            if(buffer.type & texture_mask) {
                DENG::TextureReference texture;
                texture.name = m_parser.GetProperties().model + "_TEX" + std::to_string(m_texture_bookmark);
                m_texture_bookmark++;
                texture.shader_sampler_id = 4; // const value

                // not raw texture data
                if(!(buffer.type & LIBDAS_BUFFER_TYPE_TEXTURE_RAW)) {
                    Libdas::TextureReader reader(buffer.data_ptrs.back().first, buffer.data_ptrs.back().second);
                    int x, y;
                    size_t len;
                    const char *data = reader.GetRawBuffer(x, y, len);
                    m_texture_ids.push_back(m_renderer.PushTextureFromMemory(texture, data, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4));
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
                    m_texture_ids.push_back(m_renderer.PushTextureFromMemory(texture, data, width, height, static_cast<uint32_t>(bit_depth)));
                }
            } else {
                m_renderer.UpdateCombinedBuffer(std::make_pair(buffer.data_ptrs.back().first, static_cast<uint32_t>(buffer.data_ptrs.back().second)), abs_offset);
                abs_offset += buffer.data_ptrs.back().second;
            }
        }
    }


    void ModelLoader::_CheckMeshPrimitives(const Libdas::DasMesh &_mesh) {
        BufferType base_mask = 0; // uninitialised
        DENG_ASSERT(_mesh.primitive_count);

        for(uint32_t i = 0; i < _mesh.primitive_count; i++) {
            BufferType secondary_mask = 0;
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(_mesh.primitives[i]);

            // mask creation
            if(prim.vertex_buffer_id != UINT32_MAX)
                secondary_mask |= LIBDAS_BUFFER_TYPE_VERTEX;
            if(prim.uv_buffer_id != UINT32_MAX)
                secondary_mask |= LIBDAS_BUFFER_TYPE_TEXTURE_MAP;
            if(prim.vertex_normal_buffer_id != UINT32_MAX)
                secondary_mask |= LIBDAS_BUFFER_TYPE_VERTEX_NORMAL;
            if(prim.vertex_tangent_buffer_id != UINT32_MAX)
                secondary_mask |= LIBDAS_BUFFER_TYPE_VERTEX_TANGENT;

            if(!base_mask) {
                base_mask = secondary_mask;
            } else if(base_mask != secondary_mask) {
#ifdef _DEBUG
                std::cerr << "Mesh primitive attributes in mesh are not uniform" << std::endl;
#endif
                DENG_ASSERT(false);
            }
        }
    }


    void ModelLoader::Attach() {
        _AttachBuffersAndTextures();
        //uint32_t rel_ubo_offset = m_base_ubo_offset;

        // create mesh loader instances
        for(uint32_t i = 0; i < m_parser.GetMeshCount(); i++) {
            const Libdas::DasMesh &mesh = m_parser.AccessMesh(i);
            
            // only similar attributes per mesh across primitives are allowed
            _CheckMeshPrimitives(mesh);
            //uint32_t shader_id = ModelShaderManager::RequestShaderModule(m_renderer, m_parser, m_parser.AccessMeshPrimitive(mesh.primitives[0]), m_base_ubo_offset, );

            // attach mesh
            //m_mesh_loaders.emplace_back(mesh, m_parser, m_renderer, shader_id);
            //m_mesh_loaders.back().Attach();
        }

        
        uint32_t sample_shader_id = m_mesh_loaders.front().GetShaderId(); // sample dummy shader id to use for updating uniforms
        
        // create animation sampler instances
        for(uint32_t i = 0; i < m_parser.GetAnimationCount(); i++) {
            const Libdas::DasAnimation &ani = m_parser.AccessAnimation(i);
            m_animations.emplace_back(ani.name);
            m_animations.back().samplers.reserve(ani.channel_count);

            for(uint32_t j = 0; j < ani.channel_count; j++) {
                //const Libdas::DasAnimationChannel &channel = m_parser.AccessAnimationChannel(ani.channels[j]);
                //m_animations.back().samplers.emplace_back(channel, m_parser, sample_shader_id, MODEL_SHADER_ANIMATION_UBO_BINDING);
            }
        }
    }

    
    void ModelLoader::Update(const ModelCameraUbo &_camera) { 
        // i don't need another compiler warning with -Wall okay
        DENG_ASSERT(_camera.projection_mode);
        std::cout << "Empty update function body desu" << std::endl;
    }
}
