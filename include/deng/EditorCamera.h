// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.h - Editor camera class header
// author: Karl-Mihkel Ott


#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H


#ifdef EDITOR_CAMERA_CPP
    #include <string>
    #include <cstring>
    #include <vector>
    #include <variant>
    #include <chrono>
    #include <cmath>
    #include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/ModelUniforms.h"
    #include "deng/Camera3D.h"
#endif

namespace DENG {

    class DENG_API EditorCamera : public Camera3D {
        private:
            bool m_is_enabled = false;
            TRS::Point3D<float> m_origin = { 0.0f, 0.0f, 0.0f };

            // camera TR properties
            TRS::Point3D<float> m_translation = { 0.0f, 0.0f, 0.0f };
            TRS::Point3D<float> m_rotation = { 0.0f, 0.0f, 0.0f };

        private:
            void _ForceLimits();
            void _ConstructViewMatrix();

        public:
            EditorCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name);

            virtual void EnableCamera() override;
            virtual void DisableCamera() override;
            virtual void Update() override;
    };
}

#endif
