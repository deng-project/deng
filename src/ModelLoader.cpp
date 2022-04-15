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
        m_parser.Parse();
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
                const std::string texture_name = m_parser.GetProperties().model + "_TEX" + std::to_string(m_texture_names.size());
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
                m_renderer.UpdateCombinedBuffer(std::make_pair(buffer.data_ptrs.back().first, static_cast<uint32_t>(buffer.data_ptrs.back().second)), abs_offset);
                abs_offset += buffer.data_ptrs.back().second;
            }
        }

        m_used_base_buffer_memory = static_cast<uint32_t>(abs_offset - m_base_buffer_offset);
    }


    void ModelLoader::_AttachNodeAndSkeletonTransforms(uint32_t _scene_id) {
        const Libdas::DasScene &scene = m_parser.AccessScene(_scene_id);
        
        for(uint32_t i = 0; i < scene.root_count; i++) {
            // traverse the node tree
            std::stack<std::pair<uint32_t, Libdas::Matrix4<float>>> st;
            st.push(std::make_pair(scene.roots[i], m_parser.AccessNode(scene.roots[i]).transform));

            while(!st.empty()) {
                const Libdas::DasNode &node = m_parser.AccessNode(st.top().first);
                const Libdas::Matrix4<float> mat = st.top().second;
                st.pop();

                for(uint32_t i = 0; i < node.children_count; i++)
                    st.push(std::make_pair(node.children[i], node.transform * mat));

                // submit the transformation matrix to the mesh if mesh exists for current node
                if(node.mesh != UINT32_MAX)
                    m_mesh_loaders[node.mesh].SetNodeTransform(mat);
            }
        }

        // SKELETON transformation are not yet implemented
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


    std::vector<uint32_t> ModelLoader::_GetMeshUboOffsetsFromNodeId(uint32_t _node_id) {
        std::vector<uint32_t> offsets;
        const Libdas::DasNode &node = m_parser.AccessNode(_node_id);
        offsets.push_back(m_mesh_ubo_offsets[node.mesh]);

        // flag meshes for animating
        m_mesh_loaders[node.mesh].SetAnimationTargetFlag(true);

        for(uint32_t i = 0; i < node.children_count; i++) {
            std::vector<uint32_t> sub_node_offsets = _GetMeshUboOffsetsFromNodeId(node.children[i]);
            offsets.insert(offsets.end(), sub_node_offsets.begin(), sub_node_offsets.end());
        }

        return offsets;
    }


    void ModelLoader::Attach() {
        _AttachBuffersAndTextures();
        uint32_t rel_ubo_offset = m_base_ubo_offset + m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(ModelCameraUbo)));

        const uint32_t aligned_model_ubo_size = m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(ModelUbo)));
        const uint32_t aligned_animation_ubo_size = m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(ModelAnimationUbo)));

        // create mesh loader instances
        for(uint32_t i = 0; i < m_parser.GetMeshCount(); i++) {
            const Libdas::DasMesh &mesh = m_parser.AccessMesh(i);
            
            // only similar attributes per mesh across primitives are allowed
            _CheckMeshPrimitives(mesh);
            uint32_t shader_id = ModelShaderManager::RequestShaderModule(m_renderer, m_parser, m_parser.AccessMeshPrimitive(mesh.primitives[0]), m_base_buffer_offset, m_base_ubo_offset);

            // attach mesh
            m_mesh_loaders.emplace_back(mesh, m_parser, m_renderer, shader_id, rel_ubo_offset);
            m_mesh_loaders.back().Attach();
            m_mesh_ubo_offsets.push_back(rel_ubo_offset);
            rel_ubo_offset += aligned_model_ubo_size + aligned_animation_ubo_size;
        }

        
        // create animation sampler instances
        for(uint32_t i = 0; i < m_parser.GetAnimationCount(); i++) {
            const Libdas::DasAnimation &ani = m_parser.AccessAnimation(i);
            m_animations.emplace_back(ani.name);
            m_animations.back().samplers.reserve(ani.channel_count);

            for(uint32_t j = 0; j < ani.channel_count; j++) {
                const Libdas::DasAnimationChannel &channel = m_parser.AccessAnimationChannel(ani.channels[j]);
                std::vector<uint32_t> offsets = _GetMeshUboOffsetsFromNodeId(channel.node_id);
                m_animations.back().samplers.emplace_back(channel, m_parser, offsets);
                m_animations.back().samplers.back().Animate(true);
            }
        }

        _AttachNodeAndSkeletonTransforms(0);
    }

    
    void ModelLoader::Update() { 
        for(uint32_t i = 0; i < m_parser.GetMeshCount(); i++) {
            const uint32_t rel = m_renderer.AlignUniformBufferOffset(sizeof(ModelAnimationUbo));
            ModelUbo model_ubo;
            model_ubo.node_transform = m_mesh_loaders[i].GetNodeTransform();
            model_ubo.skeleton_transform = m_mesh_loaders[i].GetSkeletonTransform();
            model_ubo.color = m_mesh_loaders[i].GetColor();
            model_ubo.use_color = true;
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(&model_ubo), static_cast<uint32_t>(sizeof(ModelUbo)), m_mesh_ubo_offsets[i] + rel);

            // tmp solution
            if(!m_mesh_loaders[i].IsAnimationTarget()) {
                ModelAnimationUbo animation_ubo;
                m_renderer.UpdateUniform(reinterpret_cast<const char*>(&animation_ubo), static_cast<uint32_t>(sizeof(ModelAnimationUbo)), m_mesh_ubo_offsets[i]);
            }
        }

        // update animations
        if(m_animations.size()) {
            for(auto ani = m_animations.begin(); ani != m_animations.end(); ani++) {
                for(auto smp = ani->samplers.begin(); smp != ani->samplers.end(); smp++) {
                    smp->Update(m_renderer);
                }
            }
        }
    }
}
