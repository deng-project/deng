// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RenderState.h - Global renderer counter class header
// author: Karl-Mihkel Ott

#ifndef RENDER_STATE_H
#define RENDER_STATE_H


#ifdef RENDER_STATE_CPP
    #include <vector>
    #include <utility>
    #include <cstdint>
    
    #include <ErrorDefinitions.h>
#endif

namespace DENG {

    enum RendererType {
        RENDERER_TYPE_OPENGL,
        RENDERER_TYPE_VULKAN,
        RENDERER_TYPE_DIRECTX,
        RENDERER_TYPE_UNKNOWN
    };

    class RenderState {
        private:
            static RenderState *m_instance;
            RendererType m_primary = RENDERER_TYPE_UNKNOWN;
            // first: RendererType specifying the backend
            // second: Timestamp of initialization
            std::vector<std::pair<RendererType, uint64_t>> m_registered_renderers;

        private:
            RenderState() = default;

        public:
            static inline RenderState *GetInstance() {
                if(!m_instance)
                    m_instance = new RenderState;
                return m_instance;
            }

            static inline void DeleteInstance() {
                delete m_instance;
            }

            inline void SetPrimary(RendererType _type) {
                m_primary = _type;
            }

            inline RendererType GetPrimary() {
                return m_primary;
            }


            inline uint32_t RegisterRenderer(RendererType _type, uint64_t _ts) {
                m_registered_renderers.push_back(std::make_pair(_type, _ts));
                return static_cast<uint32_t>(m_registered_renderers.size() - 1);
            }

            inline void RemoveRenderer(uint32_t _id) {
                DENG_ASSERT(_id < m_registered_renderers.size());
                m_registered_renderers.erase(m_registered_renderers.begin() + _id);
            }

            inline std::vector<std::pair<RendererType, uint64_t>> &GetRegisteredRenderers() {
                return m_registered_renderers;
            }

            inline const std::vector<std::pair<RendererType, uint64_t>> &GetRegisteredRenderers() const {
                return m_registered_renderers;
            }
    };
}

#endif
