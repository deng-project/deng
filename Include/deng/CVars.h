// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CVars.h - CVars classes header
// author: Karl-Mihkel Ott

#ifndef CVARS_H
#define CVARS_H

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <iostream>
#include "deng/SID.h"
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef CVARS_CPP
#include "deng/ErrorDefinitions.h"
#include <queue>
#include <fstream>
#include <stack>
#endif

namespace DENG {

	class CVar_String {
		private:
			std::string m_string;
			hash_t m_hshString = 0;

		public:
			CVar_String() = default;
			CVar_String(const std::string& _str) :
				m_string(_str),
				m_hshString(RUNTIME_CRC(m_string)) {}
			CVar_String(const char* _szString) :
				m_string(_szString),
				m_hshString(RUNTIME_CRC(m_string)) {}
			CVar_String(const CVar_String&) = default;
			CVar_String(CVar_String&&) = default;

			operator std::string& () { return m_string; }
			operator const std::string& () const { return m_string; }

			bool operator==(const CVar_String& _str) const {
				return m_hshString == _str.Hash();
			}

			void operator=(const std::string& _str) {
				m_string = _str;
				m_hshString = RUNTIME_CRC(m_string);
			}

			void operator=(const char* _szStr) {
				m_string = _szStr;
				m_hshString = RUNTIME_CRC(m_string);
			}

			void operator=(const CVar_String& _str) {
				m_string = _str.m_string;
				m_hshString = _str.m_hshString;
			}

			inline const std::string& String() const { return m_string; }
			inline hash_t Hash() const { return m_hshString; }
	};

	inline std::ostream& operator<<(std::ostream& _stream, const CVar_String& _str) {
		_stream << _str.String();
		return _stream;
	}
}

template <>
struct std::hash<DENG::CVar_String> {
	inline std::size_t operator()(const DENG::CVar_String& _str) const {
		return _str.Hash();
	}
};

namespace DENG {
	typedef int32_t CVar_Int;
	typedef float CVar_Float;
	typedef bool CVar_Bool;

	class CVar_List;
	class CVar_Object;
	typedef std::variant<std::monostate, CVar_Int, CVar_Float, CVar_Bool, CVar_String, std::shared_ptr<CVar_List>, std::shared_ptr<CVar_Object>> CVar_ListItem;

	class CVar_List {
		private:
			std::vector<CVar_ListItem> m_items;

		public:
			CVar_List() = default;
			CVar_List(const CVar_List& _list) : 
				m_items(_list.m_items) {}
			CVar_List(CVar_List&& _list) :
				m_items(std::move(_list.m_items)) {}
			CVar_List(std::initializer_list<CVar_ListItem> _initList) {
				m_items.reserve(_initList.size());
				for (auto it = _initList.begin(); it != _initList.end(); it++) {
					m_items.push_back(*it);
				}
			}

			template <typename T>
			inline void PushBack(const T& _val) { m_items.push_back(_val); }
			inline size_t Size() const { return m_items.size(); }
			inline auto Begin() const { return m_items.begin(); }
			inline auto End() const { return m_items.end(); }
			inline auto ReverseBegin() const { return m_items.rbegin(); }
			inline auto ReverseEnd() const { return m_items.rend(); }
	};

	typedef std::variant<std::monostate, CVar_Int, CVar_Float, CVar_Bool, CVar_String, CVar_List, std::shared_ptr<CVar_Object>> CVar_Value;
	struct CVar_Descriptor {
		CVar_String description;
		CVar_Value val;
	};

	class CVar_Object {
		private:
			using _Content = std::unordered_map<CVar_String, CVar_Descriptor>;

			// .first - comment/help text
			// .second - CVar_Value to use
			_Content m_contents;
		public:
			CVar_Object() = default;

			inline void PushNode(const CVar_String& _key, const CVar_String& _description, const CVar_Value& _val) {
				m_contents.emplace(std::make_pair(_key, CVar_Descriptor{ _description, _val }));
			}

			inline _Content& GetContents() { return m_contents; }
			inline const _Content& GetContents() const { return m_contents; }
	};

	enum CVarType : size_t {
		CVarType_None,
		CVarType_Int,
		CVarType_Float,
		CVarType_Bool,
		CVarType_String,
		CVarType_List,
		CVarType_Object
	};

	class CVarSystem {
		private:
			std::unordered_map<CVar_String, CVar_Descriptor> m_root;

		private:
			CVarSystem() = default;
			std::vector<CVar_String> _HashKeyWords(const std::string& _key);
			CVar_Descriptor* _FindNode(const std::string& _key);

		public:
			static CVarSystem& GetInstance();

			template <typename T>
			void Serialize(const std::string& _sFileName) {
				T serializer;
				serializer.Serialize(m_tree, _sFileName);
			}

			template <typename T>
			void Unserialize(const std::string& _sFileName) {
				T unserializer;
				unserializer.Unserialize(m_tree, _sFileName);
			}

			inline CVar_String* GetDescription(const std::string& _key) {
				CVar_Descriptor* pNode = _FindNode(_key);
				if (pNode)
					return &pNode->description;
				return nullptr;
			}

			inline CVar_Value* GetValue(const std::string& _key) {
				CVar_Descriptor* pNode = _FindNode(_key);
				if (pNode)
					return &pNode->val;
				return nullptr;
			}
			
			template<typename T>
			inline T* Get(const std::string& _key) {
				CVar_Descriptor* pNode = _FindNode(_key);
				if (pNode)
					return std::get_if<T>(&pNode->val);
				return nullptr;
			}

			template<typename T>
			bool Set(const CVar_String& _key, const CVar_String& _description, const T& _val) {
				std::vector<CVar_String> cvarStrings = _HashKeyWords(_key);
				std::unordered_map<CVar_String, CVar_Descriptor>* pTable = &m_root;

				for (size_t i = 0; i < cvarStrings.size() - 1; i++) {
					if (pTable->find(cvarStrings[i]) == pTable->end()) {
						pTable->insert(std::make_pair(cvarStrings[i], CVar_Descriptor{
							CVar_String(),
							std::make_shared<CVar_Object>()
							}));
					}
					auto pObject = std::get_if<std::shared_ptr<CVar_Object>>(&pTable->find(cvarStrings[i])->second.val);
					if (!pObject) return false;
					pTable = &pObject->get()->GetContents();
				}

				pTable->insert(std::make_pair(cvarStrings.back(), CVar_Descriptor{ _description, _val }));

				return true;
			}
	};


	inline std::ostream& operator<<(std::ostream& _stream, CVar_String& _str) {
		_stream << _str.String();
		return _stream;
	}

	// console syntax
	std::ostream& operator<<(std::ostream& _stream, CVar_List& _str);
	std::ostream& operator<<(std::ostream& _stream, CVar_Object& _obj);
}

#endif