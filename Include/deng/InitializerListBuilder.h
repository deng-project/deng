#ifndef INITIALIZER_LIST_BUILDER_H
#define INITIALIZER_LIST_BUILDER_H

namespace DENG {
	template <typename T, typename... Args>
	class InitializerListBuilder {
		private:
			T m_object;

		public:
			InitializerListBuilder(Args&&... args)
				: m_object(std::forward<Args>(args)...) {}
			InitializerListBuilder(const T& _object) :
				m_object(_object) {}

			inline T Get() {
				return m_object;
			}
	};
}

#endif
