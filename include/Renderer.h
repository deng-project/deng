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


    struct DrawCommand {
        uint32_t vertices_offset = 0;
        uint32_t indices_offset = 0;
        uint32_t indices_count = 0;
        uint32_t texture_id = UINT32_MAX;

        struct {
            Libdas::Point2D<int32_t> offset = { INT32_MAX, INT32_MAX };
            Libdas::Point2D<uint32_t> ext = { INT32_MAX, INT32_MAX };
            bool enabled = false;
        } scissor;
    };


    struct MeshReference {
        std::string name = "Mesh";
        uint32_t shader_module_id = 0;
        std::vector<DrawCommand> commands;
    };


    struct TextureReference {
        std::string name = "Texture";
        uint32_t shader_module_id = 0;
        uint32_t shader_sampler_id = 0;
        uint32_t r_identifier = UINT32_MAX;      // leave it as blank, used by renderer for referencing

        // compare by shader_sampler_id and shader_module_id
        struct less {
            bool operator()(const TextureReference &_t1, const TextureReference &_t2) {
                if(_t1.shader_module_id == _t2.shader_module_id)
                    return _t1.shader_sampler_id < _t2.shader_sampler_id;
                else return _t1.shader_module_id < _t2.shader_module_id;
            }
        };
    };


    class Renderer {
        protected:
            const Window &m_window;
            std::vector<MeshReference> m_meshes;
            std::vector<TextureReference> m_textures;
            std::vector<ShaderModule> m_shaders;

        public:
            Renderer(const Window &_win) : m_window(_win) {}
            ~Renderer() {}

            inline uint32_t PushMeshReference(const MeshReference &_mesh) {
                m_meshes.push_back(_mesh);
                return static_cast<uint32_t>(m_meshes.size() - 1);
            }

            inline uint32_t PushShader(const ShaderModule &_module) {
                m_shaders.push_back(_module);
                return static_cast<uint32_t>(m_shaders.size() - 1);
            }

            virtual uint32_t PushTextureFromFile(const DENG::TextureReference &_tex, const std::string &_file_name) = 0;
            virtual uint32_t PushTextureFromMemory(const DENG::TextureReference &_tex, const char* _raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) = 0;

            std::vector<MeshReference> &GetMeshes() {
                return m_meshes;
            }

            std::vector<ShaderModule> &GetShaderModules() {
                return m_shaders;
            }

            std::vector<TextureReference> &GetTextures() {
                return m_textures;
            }

            virtual void ShrinkTextures() = 0;
            virtual void LoadShaders() = 0;
            virtual void UpdateUniform(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) = 0;
            virtual void UpdateVertexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void UpdateIndexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) = 0;
            virtual void ClearFrame() = 0;
            virtual void RenderFrame() = 0;
    };
}

#endif
