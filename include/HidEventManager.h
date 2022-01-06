// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: HidEventManager.h - Human interface device handling class header
// author: Karl-Mihkel Ott

#ifndef HID_EVENT_MANAGER_H
#define HID_EVENT_MANAGER_H


#if 0
#include <string>
#include <vector>

#include <BaseTypes.h>
#include <libdas/include/Points.h>
#include <Window.h>
#endif

namespace DENG {

    template<class T>
    struct HidEvent {
        typedef void (T::*PFN_Callback)(void *_user_data);
        HidEvent(neko_InputBits _in_bits, neko_InputEventType _ev_type, PFN_Callback _callback, void *_user_data) :
            input_bits(_in_bits), ev_type(_ev_type), callback(_callback), user_data(_user_data) {}

        neko_InputBits input_bits;
        neko_InputEventType ev_type;
        PFN_Callback callback;
        void *user_data;
    };


    template <class T>
    class HidEventManager {
        public:
            typedef void (T::*PFN_Callback)(void *_user_data);

        private:
            const Window &m_win;
            std::vector<HidEvent<T>> m_actions;

        public:
            HidEventManager(const Window &_win) : m_win(_win) {}

            inline void RegisterEvent(neko_InputBits _event_bits, neko_InputEventType _ev_type, PFN_Callback _callback) {
                m_actions.push_back(HidEvent<T>(_event_bits, _ev_type, _callback));
            }

            /// Update function to check if any events should occur
            void Update() {
                for(auto it = m_actions.begin(); it != m_actions.end(); it++) {
                    neko_HidEvent *keycodes = reinterpret_cast<neko_HidEvent*>(&it->input_bits);

                    bool match = true;
                    // check all possible keycodes
                    for(uint8_t i = 0; i < 8; i++) {
                        if(keycodes[i] == NEKO_HID_UNKNOWN || !neko_FindKeyStatus(keycodes[i], it->ev_type)) {
                            match = false;
                            break;
                        }
                    }

                    if(match) it->callback(it->data);
                }
            }
    };
}

#endif
