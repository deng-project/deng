#include <iostream>
#include "deng/ResourceIdTable.h"
#include <iomanip>

using namespace DENG;

dDECLARE_RESOURCE_ID_TABLE(ctm)
	dRESOURCE_ID_ENTRY("Someidentifier1"),
	dRESOURCE_ID_ENTRY("Someidentifier2"),
	dRESOURCE_ID_ENTRY("Someidentifier3"),
	dRESOURCE_ID_ENTRY("Someidentifier4")
dEND_RESOURCE_ID_TABLE(ctm)

int main(void) {

	constexpr bool has_false = ctm{}.Has<Wrapper<SID("false")>>();
	std::cout << (has_false ? "true" : "false") << std::endl;
	
	auto rtm = ctm{}.MakeUnorderedMap<hash_t, const char*>();
	for (auto it = rtm.begin(); it != rtm.end(); it++)
		std::cout << "{ " << it->first << ": " << it->second << " }\n";
}
