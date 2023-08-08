#include <iostream>
#include "deng/ErrorDefinitions.h"
#include "deng/CVars.h"

using namespace std;
using namespace DENG;

void PrintVariable(CVar_Value* _pValue) {
	switch (_pValue->index()) {
		case CVarType_Bool:
			cout << (std::get<CVar_Bool>(*_pValue) ? "true" : "false") << '\n';
			break;

		case CVarType_Float:
			cout << std::get<CVar_Float>(*_pValue) << '\n';
			break;

		case CVarType_Int:
			cout << std::get<CVar_Int>(*_pValue) << '\n';
			break;

		case CVarType_List:
			cout << std::get<CVar_List>(*_pValue) << '\n';
			break;

		case CVarType_Object:
			cout << *std::get<std::shared_ptr<CVar_Object>>(*_pValue).get() << '\n';
			break;

		default:
			DENG_ASSERT(false);
			break;
	}
}

int main(void) {
	CVarSystem& cvarSystem = CVarSystem::GetInstance();
	cvarSystem.Set<CVar_Int>("lulz", "Some int property", 69);
	cvarSystem.Set<CVar_Float>("engine.someFloat", "This is a float property", 0.981237f);
	cvarSystem.Set<CVar_String>("engine.someString", "This is a string property", "Some string value lol");
	cvarSystem.Set<CVar_Int>("engine.someInt", "This is an int property", 0);
	cvarSystem.Set<CVar_Int>("engine.someSection1.someSection2.someSection3.value", "", 123);
	cvarSystem.Set<CVar_Int>("engine.someSection1.someSection2.someSection3.value1", "", 1234);
	cvarSystem.Set<CVar_List>("environment.color", "Basically an environment color\nThis value is irrelevant if using a cubemap", CVar_List{ 1.f, 1.f, 1.f, 1.f });
	
	cout << "Welcome to CVar interactive console!\n"\
			"Type '<variable> = <value>' to set or overwrite a new value\n"\
			"Type '<variable>' to read a value\n";
	
	while (true) {
		string line;
		getline(cin, line);

		// check if the program should exit
		if (line == "exit" || line == "quit")
			break;

		size_t i = line.find('=', 0);
		if (i == std::string::npos) {
			auto pVal = cvarSystem.GetValue(line);
			if (!pVal) {
				cout << "Invalid variable '" << line << "'\n";
				continue;
			}

			PrintVariable(pVal);
		}
	}
	return 0;
}