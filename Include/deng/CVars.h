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
#endif

namespace DENG {

	struct CVar_Color {
		CVar_Color(
			float _r,
			float _g,
			float _b,
			float _a) :
			r(_r), g(_g), b(_b), a(_a) {}

		float r; float g; float b; float a;
	};

	inline std::ostream& operator<<(std::ostream& _stream, const CVar_Color& _color) {
		int r = int(255 * _color.r);
		int g = int(255 * _color.g);
		int b = int(255 * _color.b);
		int a = int(255 * _color.a);
		_stream << "[ " << r << ", " << g << ", " << b << ", " << a << " ]";
		return _stream;
	}

	inline std::istream& operator>>(std::istream& _stream, CVar_Color& _color) {
		int r = 0, g = 0, b = 0, a = 0;
		char str[1024]{0};
		_stream.readsome(str, 1024);
		(void) std::sscanf(str, "[ %d, %d, %d, %d ]", &r, &g, &b, &a);
		_color.r = static_cast<float>(r) / 255.f;
		_color.g = static_cast<float>(g) / 255.f;
		_color.b = static_cast<float>(b) / 255.f;
		_color.a = static_cast<float>(a) / 255.f;
		return _stream;
	}


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
	
			operator std::string&() { return m_string; }
			operator const std::string&() const { return m_string; }
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

	typedef int32_t CVar_Int;
	typedef float CVar_Float;
	typedef bool CVar_Bool;

	struct CVar_Description {
		CVar_String str;

		CVar_Description() = default;
		CVar_Description(const CVar_String& _str) :
			str(_str) {}
	};

	std::ostream& operator<<(std::ostream& _stream, CVar_Description& _desc);

	template<typename T>
	class CVar {
		private:
			CVar_String m_key;
			CVar_Description m_description;
			T m_value;

		public:
			CVar(const CVar_String& _key, const CVar_String& _desc, const T& _val) :
				m_key(_key),
				m_description(_desc),
				m_value(_val) {}
			CVar() = default;

			inline CVar_String& GetKey() { return m_key; }
			inline CVar_Description& GetDescription() { return m_description; }
			inline const T& GetValue() { return m_value; }
			inline void Set(const T& _val) { m_value = _val; }
	};

	template <typename T>
	std::string MakeCVarString(const CVar_String& _sKey, const CVar_String& _sDesc, const T& _val) {
		// find the indentation level
		size_t uIndent = std::count(_sKey.String().begin(), _sKey.String().end(), '.');
		std::stringstream sStream;
		
		bool bNextIndent = false;
		for (size_t i = 0; i < _sDesc.String().size(); i++) {
			if (_sDesc.String()[i] != '\n' && i) {
				if (bNextIndent) {
					sStream << std::setfill('\t') << std::setw(uIndent) << "" << "# ";
					bNextIndent = false;
				}
				sStream << _sDesc.String()[i];
			}
			else if (!i)
				sStream << std::setfill('\t') << std::setw(uIndent) << "" << "# " << _sDesc.String()[i];
			else {
				bNextIndent = true;
				sStream << _sDesc.String()[i];
			}
		}

		if (_sDesc.String().size() && _sDesc.String().back() != '\n')
			sStream << '\n';

		const std::string sNodeName = (_sKey.String().rfind('.') == std::string::npos ? _sKey.String() : _sKey.String().substr(_sKey.String().rfind('.') + 1));
		sStream << std::setfill('\t') << std::setw(uIndent) << "" << sNodeName << ": " << _val;
	
		return sStream.str();
	}

	template<typename T>
	inline std::ostream& operator<<(std::ostream& _stream, CVar<T>& _val) {
		_stream << MakeCVarString(_val.GetKey(), _val.GetDescription().str, _val.GetValue());
		return _stream;
	}


	struct TrieNode {
		CVar_String sId;
		hash_t hshParent = 0;
		std::unordered_map<hash_t, TrieNode, NoHash> childNodes;
		std::variant<std::monostate, CVar<CVar_Int>, CVar<CVar_Float>, CVar<CVar_Bool>, CVar<CVar_String>, CVar<CVar_Color>> value;
	};

	class CVarSystem {
		private:
			using _Variant_T = std::variant<std::monostate, CVar<CVar_Int>, CVar<CVar_Float>, CVar<CVar_Bool>, CVar<CVar_String>, CVar<CVar_Color>>;
			std::unordered_map<hash_t, TrieNode, NoHash> m_rootNodes;

		private:
			CVarSystem() = default;
			std::vector<CVar_String> _HashKeyWords(const std::string& _key);
			TrieNode* _FindNode(const std::string& _key);

		public:
			static CVarSystem& GetInstance();

			void Serialize(const std::string& _sFileName);
			void Unserialize(const std::string& _sFileName);
			
			template<typename T>
			CVar<T>* Get(const std::string& _key) {
				TrieNode* pNode = _FindNode(_key);
				if (pNode)
					return std::get_if<CVar<T>>(&pNode->value);
				return nullptr;
			}

			template<typename T>
			CVar<T>* Set(const CVar_String& _key, const CVar_String& _desc, const T& _val) {
				std::vector<CVar_String> cvarStrings = _HashKeyWords(_key);
				std::unordered_map<hash_t, TrieNode, NoHash>* pTable = &m_rootNodes;

				hash_t hshPrev = 0;
				for (size_t i = 0; i < cvarStrings.size() - 1; i++) {
					if (i) hshPrev = cvarStrings[i - 1].Hash();

					if (pTable->find(cvarStrings[i].Hash()) == pTable->end()) {
						pTable->insert(std::make_pair(cvarStrings[i].Hash(), TrieNode{
							cvarStrings[i],
							hshPrev,
							std::unordered_map<hash_t, TrieNode, NoHash>(),
							std::monostate{}
							}));
					}
					pTable = &pTable->find(cvarStrings[i].Hash())->second.childNodes;
				}

				if (cvarStrings.size() > 1)
					hshPrev = cvarStrings[cvarStrings.size() - 2].Hash();

				pTable->insert(std::make_pair(cvarStrings.back().Hash(), TrieNode {
					cvarStrings.back(),
					hshPrev,
					std::unordered_map<hash_t, TrieNode, NoHash>(),
					CVar<T>(_key, _desc, _val)
					}));

				return std::get_if<CVar<T>>(&pTable->find(cvarStrings.back().Hash())->second.value);
			}
	};
}

#endif