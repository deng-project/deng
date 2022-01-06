// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.h - 3D camera base class
// author: Karl-Mihkel Ott

#ifndef CAMERA_3D_H
#define CAMERA_3D_H


namespace DENG {

    class Camera3D : private HidEventManager<Camera3DControl> {
        private:
            uint32_t m_camera_id = 0;
            std::string m_name;

        public:
            Camera3D(const Window &_win, const std::string &_name, uint32_t &_registry_id) : 
                HidEventManager<Camera3DControl>(_win), m_camera_id(_registry_id), m_name(_name) 
            { 
                _registry_id++; 
            }

            inline const std::string &GetName() {
                return m_name;
            }

            inline uint32_t GetCameraId() {
                return m_camera_id;
            }

            virtual void Attach(Renderer &_rend) = 0;
    };
}

#endif
