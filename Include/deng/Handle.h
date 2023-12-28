// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Handle.h - Generic handle classes header
// author: Karl-Mihkel Ott

#pragma once

namespace DENG
{
	template <typename T>
	class Handle
	{
		private:
			T* m_pData = nullptr;
			size_t* m_pReference = nullptr;

		public:
			Handle() = default;
			Handle(T* pValue) : m_pData(pValue)
			{
				m_pReference = new size_t;
				*m_pReference = 1;
			}

			Handle(const Handle<T>& _handle) : 
				m_pData(_handle.m_pData),
				m_pReference(_handle.m_pReference)
			{
				if (m_pReference)
				{
					++(*m_pReference);
				}
			}

			~Handle()
			{
				--(*m_pReference);
				if (m_pData && *m_pReference == 0)
				{
					delete m_pData;
					delete m_pReference;
				}
			}

			T& operator*()
			{
				return *m_pDatas;
			}

			T* operator->()
			{
				return m_pData;
			}

			Handle<T>& operator=(const Handle<T>& _handle)
			{
				if (this != &_handle)
				{
					// decrement old reference count
					if (m_pReference)
						--(*m_pReference);
					if (m_pData && *m_pReference == 1)
					{
						delete m_pData;
						delete m_pReference;
					}

					m_pData = _handle.m_pData;
					m_pReference = _handle.m_pReference;
					(*m_pReference)++;
				}

				return *this;
			}


	};
}
