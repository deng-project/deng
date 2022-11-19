#ifndef INPUT_REGISTRY_H
#define INPUT_REGISTRY_H

namespace DENG {

	class InputRegistry {
		private:
			std::array<bool, NEKO_HID_EVENT_COUNT> m_active_table;
			std::array<bool, NEKO_HID_EVENT_COUNT> m_released_table;

		public:
			InputRegistry() {
				std::fill(m_active_table.begin(), m_active_table.end(), false);
				std::fill(m_released_table.begin(), m_released_table.end(), false);
			}

			inline void RegisterActiveKeyEvent(neko_HidEvent _hid) {
				m_active_table[_hid] = true;
				m_released_table[_hid] = false;
			}

			inline void RegisterReleasedKeyEvent(neko_HidEvent _hid) {
				m_active_table[_hid] = false;
				m_released_table[_hid] = true;
			}

			inline bool IsKeyActive(neko_HidEvent _hid) {
				return m_active_table[_hid];
			}

			inline bool IsKeyReleased(neko_HidEvent _hid) {
				return m_released_table[_hid];
			}

			inline void FlushReleased() {
				m_active_table[NEKO_MOUSE_SCROLL_DOWN] = false;
				m_active_table[NEKO_MOUSE_SCROLL_UP] = false;
				std::fill(m_released_table.begin(), m_released_table.end(), false);
			}
	};
}

#endif
