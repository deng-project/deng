#define ERROR_DEFINITIONS_CPP
#include "deng/ErrorDefinitions.h"

using namespace std;

namespace DENG {
	void DispatchErrorToStdErr(const string& _sExceptionType, const string& _sWhat, ErrorSeverity _eSeverity) {
		HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);

		switch (_eSeverity) {
			case CRITICAL:
			case CRITICAL_NON_FATAL:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
				cerr << '[' << _sExceptionType << "] " << _sWhat << '\n';
				break;

			case NON_CRITICAL:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
				cerr << '[' << _sExceptionType << "] " << _sWhat << '\n';
				break;

			case WARNING:
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				cerr << '[' << _sExceptionType << "] " << _sWhat << '\n';
				break;

			case INFO:
				cerr << '[' << _sExceptionType << ']' << _sWhat << '\n';
				break;
		}

		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if (_eSeverity == CRITICAL) {
			cin.get();
			exit(1);
		}
	}
}