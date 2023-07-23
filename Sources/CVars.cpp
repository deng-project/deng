// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CVars.cpp - CVars classes implementation
// author: Karl-Mihkel Ott

#define CVARS_CPP
#include "deng/CVars.h"

namespace DENG {

	std::ostream& operator<<(std::ostream& _stream, CVar_Description& _desc) {
		auto& str = _desc.str.String();
		std::queue<std::string> qDesc;
		
		size_t uPos = 0;
		size_t uBeginPos = 0;

		const char cDelim = '\n';
		while ((uPos = str.find(cDelim, uBeginPos)) != std::string::npos) {
			qDesc.push(str.substr(uBeginPos, uPos));
			uBeginPos = uPos + 1;
		}

		if (uBeginPos != str.size())
			qDesc.push(str.substr(uBeginPos));

		while (!qDesc.empty()) {
			_stream << "# " << qDesc.front() << '\n';
			qDesc.pop();
		}

		return _stream;
	}


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


	TrieNode* CVarSystem::_FindNode(const std::string& _key) {
		auto hashes = _HashKeyWords(_key);
		TrieNode* pNode = nullptr;
		std::unordered_map<hash_t, TrieNode, NoHash>* pNodeTable = &m_rootNodes;

		for (size_t i = 0; i < hashes.size(); i++) {
			auto itNode = pNodeTable->find(hashes[i].Hash());
			if (itNode != pNodeTable->end()) {
				pNodeTable = &itNode->second.childNodes;
				pNode = &itNode->second;
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

	struct _TraversalNode {
		std::unordered_map<hash_t, TrieNode, NoHash>::iterator itNode;
		size_t uNTabs = 0;
		std::unordered_map<hash_t, TrieNode, NoHash>* pTable = nullptr;
	};

	void CVarSystem::Serialize(const std::string& _sFileName) {
		std::ofstream stream(_sFileName);
		
		std::deque<_TraversalNode> qNodes;

		for (auto it = m_rootNodes.begin(); it != m_rootNodes.end(); it++) {
			qNodes.push_back({ it, 0, &m_rootNodes });
		}

		while (!qNodes.empty()) {
			auto node = qNodes.front();
			qNodes.pop_front();

			// check if the current TrieNode* instance has a value
			if (node.itNode->second.value.index() != 0) {
				switch (node.itNode->second.value.index()) {
					// CVar<CVar_Int>
					case 1:
						stream << std::get<1>(node.itNode->second.value) << '\n';
						break;

					// CVar<CVar_Float>
					case 2:
						stream << std::get<2>(node.itNode->second.value) << '\n';
						break;

					// CVar<CVar_Bool>
					case 3:
						stream << std::get<3>(node.itNode->second.value) << '\n';
						break;

					// CVar<CVar_String>
					case 4:
						stream << std::get<4>(node.itNode->second.value) << '\n';
						break;

					// CVar<CVar_Color>
					case 5:
						stream << std::get<5>(node.itNode->second.value) << '\n';
						break;

					default:
						DENG_ASSERT(false);
						break;
				}
			}
			else {
				stream << std::setfill('\t') << std::setw(node.uNTabs) << "" << node.itNode->second.sId.String() << ":\n";

				for (auto it = node.itNode->second.childNodes.begin(); it != node.itNode->second.childNodes.end(); it++)
					qNodes.push_front({ it, node.uNTabs + 1, &node.itNode->second.childNodes });
			}
		}

		stream.close();
	}

}