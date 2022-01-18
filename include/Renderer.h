// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Renderer.h - Base renderer class header
// author: Karl-Mihkel Ott

#ifndef RENDERER_H
#define RENDERER_H

namespace DENG {

    struct CameraComponent {
        typedef Libdas::Matrix4<float> Matrix;
        CameraComponent() {}
        CameraComponent(Matrix &_cam, Matrix &_view) :
            camera_matrix(_cam), camera_view_matrix(_view) {}

        Matrix camera_matrix;
        Matrix camera_view_matrix;
    };


    struct MeshReference {
        std::string name = "Mesh";
        uint32_t vertices_offset = 0;
        uint32_t indices_offset = 0;
        uint32_t indices_count = 0;
        uint32_t texture_id = 0;
        uint32_t shader_module_id = 0;
    };


    struct TextureReference {
        std::string name = "Texture";
        uint32_t buffer_offset = 0;
        uint32_t shader_module_id = 0;
    };


    class Renderer {
        protected:
            const Window &m_window;
            std::vector<MeshReference> m_meshes;
            std::vector<TextureReference> m_textures;
            std::vector<ShaderModule*> m_shaders;

        public:
            Renderer(const Window &_win) : m_window(_win) {}
            ~Renderer() {}

            inline void PushMeshReference(const MeshReference &_mesh) {
                m_meshes.push_back(_mesh);
            }

            inline void PushShader(ShaderModule *_shader) {
                m_shaders.push_back(_shader);
            }

            inline void RemoveMeshReference(uint32_t _id) {
                m_meshes.erase(m_meshes.begin() + _id);
            }

            inline void PushTextureReference(const TextureReference &_texture) {
                m_textures.push_back(_texture);
            }

            inline void RemoveTextureReference(uint32_t _id) {
                m_textures.erase(m_textures.begin() + _id);
            }

            virtual void LoadShaders() = 0;
            virtual void UpdateUniform(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) = 0;
            virtual void UpdateVertexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void UpdateIndexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
