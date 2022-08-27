#define INPUT_REGISTRY_CPP
#include "deng/InputRegistry.h"

namespace DENG {

	InputRegistry::InputRegistry() {
		std::fill(m_active_table.begin(), m_active_table.end(), false);
	}
}