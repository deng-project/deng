#include <list>
#include <vector>
#include <string>
#include <iostream>

#define DENG_API
#include <deng/GPUMemoryAllocator.h>

using namespace std;

int main(void) {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	DENG::GPUMemoryAllocator gpuMemoryAllocator;

	while (true) {
		string sArg1;
		size_t uArg2;
		cin >> sArg1 >> uArg2;


		if (sArg1 != "DEL")
			gpuMemoryAllocator.RequestMemory(static_cast<size_t>(stoll(sArg1)), uArg2);
		else
			gpuMemoryAllocator.FreeMemory(uArg2);

		gpuMemoryAllocator.DbPrintMemoryRegions();
	}

	return 0;
}