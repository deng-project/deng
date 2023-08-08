// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CVars.cpp - CVars classes implementation
// author: Karl-Mihkel Ott

#define CVARS_CPP
#include "deng/CVars.h"

namespace DENG {

	std::vector<CVar_String> CVarSystem::_HashKeyWords(const std::string& _key) {
		std::queue<std::string> qKeyWords;

		size_t uPos = 0;
		size_t uBeginPos = 0;

		const char cDelim = '.';
		while ((uPos = _key.find(cDelim, uBeginPos)) != std::string::npos) {
			qKeyWords.push(_key.substr(uBeginPos, uPos - uBeginPos));
			uBeginPos = uPos + 1;
		}

		if (uBeginPos != _key.size())
			qKeyWords.push(_key.substr(uBeginPos));

		std::vector<CVar_String> hashes;
		hashes.reserve(qKeyWords.size());

		while (!qKeyWords.empty()) {
			hashes.emplace_back(qKeyWords.front());
			qKeyWords.pop();
		}

		return hashes;
	}


	CVar_Descriptor* CVarSystem::_FindNode(const std::string& _key) {
		auto hashes = _HashKeyWords(_key);
		CVar_Descriptor* pNode = nullptr;
		std::unordered_map<CVar_String, CVar_Descriptor>* pNodeTable = &m_root;

		for (size_t i = 0; i < hashes.size(); i++) {
			auto itNode = pNodeTable->find(hashes[i]);
			if (itNode != pNodeTable->end()) {
				pNode = &itNode->second;
				auto pObject = std::get_if<std::shared_ptr<CVar_Object>>(&pNode->val);
				
				if (i != hashes.size() - 1 && !pObject)
					return nullptr;
				else if (i != hashes.size() - 1) 
					pNodeTable = &pObject->get()->GetContents();
			}
			else 
				return nullptr;
		}

		return pNode;
	}

	CVarSystem& CVarSystem::GetInstance() {
		static CVarSystem system;
		return system;
	}

	// ostream operators
	std::ostream& operator<<(std::ostream& _stream, CVar_List& _list) {
		std::stack<CVar_List*> stckList;
		stckList.push(&_list);

		while (!stckList.empty()) {
			_stream << '[';
			auto pList = stckList.top();

			for (auto it = pList->Begin(); it != pList->End(); it++) {
				switch (it->index()) {
					case CVarType_Bool:
						_stream << (std::get<CVar_Bool>(*it) ? "true" : "false");
						break;

					case CVarType_Float:
						_stream << std::get<CVar_Float>(*it);
						break;

					case CVarType_Int:
						_stream << std::get<CVar_Int>(*it);
						break;

					case CVarType_List:
						_stream << '[';
						stckList.push(std::get<std::shared_ptr<CVar_List>>(*it).get());
						break;

					case CVarType_Object:
						_stream << *std::get<std::shared_ptr<CVar_Object>>(*it).get();
						break;

					case CVarType_String:
						_stream << '\"' << std::get<CVar_String>(*it) << '\"';
						break;

					default:
						DENG_ASSERT(false);
						break;
				}

				if (it != (_list.End() - 1))
					_stream << ", ";
				else _stream << ']';
			}

			stckList.pop();
			if (!stckList.empty())
				_stream << ", ";
		}

		return _stream;
	}

	std::ostream& operator<<(std::ostream& _stream, CVar_Object& _obj) {
		std::stack<std::pair<CVar_Object*, std::unordered_map<CVar_String, CVar_Descriptor>::iterator>> stckObjects;
		stckObjects.push(std::make_pair(&_obj, _obj.GetContents().begin()));

		_stream << '{';
		while (!stckObjects.empty()) {
			BEGIN:
			auto& obj = stckObjects.top();

			for (auto it = obj.second; it != obj.first->GetContents().end(); it++) {
				_stream << '\"' << it->first << "\": ";
				switch (it->second.val.index()) {
					case CVarType_Bool:
						_stream << (std::get<CVar_Bool>(it->second.val) ? "true" : "false");
						break;

					case CVarType_Float:
						_stream << std::get<CVar_Float>(it->second.val);
						break;

					case CVarType_Int:
						_stream << std::get<CVar_Int>(it->second.val);
						break;

					case CVarType_List:
						_stream << std::get<CVar_List>(it->second.val);
						break;

					case CVarType_Object:
					{
						_stream << '{';
						auto pObject = std::get<std::shared_ptr<CVar_Object>>(it->second.val).get();
						it++;
						obj.second = it;
						stckObjects.push(std::make_pair(pObject, pObject->GetContents().begin()));
						goto BEGIN;
					}

					case CVarType_String:
						_stream << '\"' << std::get<CVar_String>(it->second.val) << '\"';
						break;

					default:
						DENG_ASSERT(false);
						break;
				}

				auto it2 = obj.first->GetContents().end();
				it2--;
				if (it != it2)
					_stream << ", ";
			}
			
			_stream << '}';
			stckObjects.pop();
		}

		return _stream;
	}
}