#include "deng/SID.h"
#include <iostream>
#include <iomanip>

int main(void) {
	std::cout << "Compile time crc64 hashes of some strings." << std::endl;
	std::cout << "\"Welcome!\": 0x" << std::setw(8) << std::hex << SID("Welcome") << std::endl;
	std::cout << "\"Hello world1!\": 0x" << std::setw(8) << std::hex << SID("Hello world1!") << std::endl;
	std::cout << "\"Hello world!\": 0x" << std::setw(8) << std::hex << SID("Hello world!") << std::endl;
	std::cout << "\"Ur a faggit\": 0x" << std::setw(8) << std::hex << SID("Ur a faggit") << std::endl;
	std::cout << "\"Ur a faggot\": 0x" << std::setw(8) << std::hex << SID("Ur a faggot") << std::endl;
	return 0;
}