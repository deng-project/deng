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
        uint64_t vertices_offset = 0;
        uint64_t indices_offset = 0;
        uint64_t indices_count = 0;
        uint64_t texture_id = 0;
    };


    struct TextureReference {
        std::string name = "Texture";
        uint64_t buffer_offset = 0;
    };


    class Renderer {
        protected:
            const Window &m_window;
            uint32_t m_active_camera_component = 0;
            std::vector<CameraComponent> m_camera_components;
            std::vector<MeshReference> m_meshes;
            std::vector<TextureReference> m_textures;

        public:
            Renderer(const Window &_win) : m_window(_win) {}
            ~Renderer() {}

            inline void PushCameraComponent(const CameraComponent &_component) {
                m_camera_components.push_back(_component);
            }

            inline void MakeCameraActive(uint32_t _id) {
                m_active_camera_component = _id;
            }

            inline void RemoveCameraComponent(uint32_t _id) {
                m_camera_components.erase(m_camera_components.begin() + _id);
            }

            inline void PushMeshReference(const MeshReference &_mesh) {
                m_meshes.push_back(_mesh);
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

            virtual void UpdateUniforms() = 0;
            virtual void UpdateVertexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset = 0) = 0;
            virtual void UpdateIndexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
