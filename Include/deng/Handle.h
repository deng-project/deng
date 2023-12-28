// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Handle.h - Generic handle classes header
// author: Karl-Mihkel Ott

#pragma once

namespace DENG
{
	template <typename T, typename... Args>
	class ObjectReference
	{
		private:
			T* m_pObject = nullptr;
			size_t m_uReferences = 0;
		
		public:
			ObjectReference(T* _pObject) :
				m_pObject(_pObject)
			{
			}

			~ObjectReference()
			{
				delete m_pObject;
			}

			void AddReference()
			{
				m_uReferences++;
			}

			size_t Release()
			{
				return --m_uReferences;
			}

			T& Get()
			{
				return *m_pObject;
			}

			T* GetPtr()
			{
				return m_pObject;
			}
	};

	template <typename T>
	class Handle
	{
		private:
			ObjectReference<T>* m_pData;

		public:
			Handle() : m_pData(nullptr)
			{
			}

			Handle(T* pValue) : m_pData(new ObjectReference<T>(pValue))
			{
			}

			Handle(const Handle<T>& _handle) : m_pData(_handle.m_pData)
			{
				m_pData->AddReference();
			}

			~Handle()
			{
				if (m_pData && m_pData->Release() == 0)
				{
					delete m_pData;
				}
			}

			T& operator*()
			{
				return *m_pData->Get();
			}

			T* operator->()
			{
				return m_pData->GetPtr();
			}

			Handle<T>& operator=(const Handle<T>& _handle)
			{
				if (this != &_handle)
				{
					// decrement old reference count
					if (m_pData and m_pData->Release() == 0)
					{
						delete m_pData;
					}

					m_pData = _handle.m_pData;
					m_pData->AddReference();
				}

				return *this;
			}


	};
}
