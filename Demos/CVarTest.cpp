#include <iostream>
#include "deng/CVars.h"

using namespace std;
using namespace DENG;

int main(void) {
CVarSystem& cvarSystem = CVarSystem::GetInstance();
cvarSystem.Set<CVar_Float>("lulz", "", 0.69f);
cvarSystem.Set<CVar_Float>("engine.someFloat", "This is a float property", 0.981237f);
cvarSystem.Set<CVar_String>("engine.someString", "This is a string property", "Some string value lol");
cvarSystem.Set<CVar_Int>("engine.someInt", "This is an int property", 0);
cvarSystem.Set<CVar_Int>("engine.someSection1.someSection2.someSection3.value", "", 123);
cvarSystem.Set<CVar_Color>("environment.color", "Basically an environment color\nThis value is irrelevant if using a cubemap", CVar_Color(0.5, 0.1, 0.33, 1.0));
cvarSystem.Serialize("deng.yml");

std::cout << "lulz = " << cvarSystem.Get<CVar_Float>("lulz")->GetValue() << '\n';
std::cout << "engine.someFloat = " << cvarSystem.Get<CVar_Float>("engine.someFloat")->GetValue() << '\n';
std::cout << "engine.someString = " << cvarSystem.Get<CVar_String>("engine.someString")->GetValue() << '\n';
std::cout << "engine.someInt = " << cvarSystem.Get<CVar_Int>("engine.someInt")->GetValue() << '\n';
std::cout << "environment.color = " << cvarSystem.Get<CVar_Color>("environment.color")->GetValue() << '\n';
return 0;
}