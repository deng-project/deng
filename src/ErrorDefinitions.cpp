#define ERROR_DEFINITIONS_CPP
#include "deng/ErrorDefinitions.h"

using namespace std;

namespace DENG {
	void DispatchErrorToStdErr(const string& _sExceptionType, const string& _sWhat, ErrorSeverity _eSeverity) {
		HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);

		switch (_eSeverity) {
			case CRITICAL:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				cerr << '[' << _sExceptionType << "] " << _sWhat << "\n\n";
				exit(0);
				break;

			case NON_CRITICAL:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
				cerr << '[' << _sExceptionType << "] " << _sWhat << "\n\n";
				break;

			case WARNING:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				cerr << '[' << _sExceptionType << "] " << _sWhat << "\n\n";
				break;
		}

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}