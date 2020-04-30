#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define CURRENT_PLATFORM_UNIX
#include <unistd.h>
#include <term.h>
#elif __WIN32
#define CURRENT_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "gui.h"
#include <thread>

namespace gui {
	bool setup() {

#if defined(CURRENT_PLATFORM_WINDOWS)

		HANDLE hStdOut;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD count;
		DWORD cellCount;
		COORD homeCoords = { 0, 0 };

		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdOut == INVALID_HANDLE_VALUE) {
			std::cout << "couldn't get terminal handle.\n";
			return false;
		}

		// get number of cells in current terminal
		if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
			std::cout << "couldn't retrieve terminal info.\n";
			return false;
		}
		cellCount = csbi.dwSize.X * csbi.dwSize.Y;

		// fill terminal with spaces
		if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count)) {
			std::cout << "couldn't fill terminal with spaces.\n";
			return false;
		}

		// fill terminal with previous attributes and colors
		if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) {
			std::cout << "couldn't fill terminal with previous attributes.\n";
			return false;
		}

		// move cursor to home coordinates and everything's alright
		SetConsoleCursorPosition(hStdOut, homeCoords);

#elif defined(CURRENT_PLATFORM_UNIX)

		// this belongs to ncurses. gotta
		// link ncurses lib to find term.h
		if (!cur_term) {
			int result;
			setupterm(NULL, STDOUT_FILENO, &result);
			if (!result) {
				std::cout << "couldn't instantiate nor find terminal.\n";
				return false;
			}
		}
		putp(tigetstr("clear"));

#else

		std::cout << "platform not supported at the moment. sorry.\n";
		return false;

#endif

		// draw presentation
		const char* idyll[] = {
			"\033[1;36m",
			"                  /           /  /\n",
			"                /           /  /  \n",
			"     o  ______/           /  /    \n",
			"   /  /     /  /     /  /  /      \n",
			" /  /     /  /     /  /  /        \n",
			"(__(____/(__(____/(__(__(__  o    \n",
			"               /                  \n",
			"             /                    \n",
			"      (____/                      \n",
			"\n",
			"\033[0;36m",
			" ~    b y        s o y b i n    ~ \n",
			"\n"
		};

		for (int i = 0; i < sizeof(idyll) / sizeof(const char*); ++i) {
			std::cout << idyll[i];
		}

		return true;
	}
	
	void update(int* curV, int tarV) {
		for (; *curV < tarV; ){
			// width of logo is 34 characters. two of which
			// will be used for the extremes of the bar.
			// terminal colors are represented with expressions
			// like "\033[x;xx"
			std::cout << "\r\033[1;36m[";
			int quantity = 1 + 32 * (*curV) / tarV;
			for (int i = 0; i < 32; ++i) {
				std::cout << (i < quantity ? "\033[1;36m=" : "\033[0m-");
			}
			std::cout << "\033[1;36m]" << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		std::cout << "\033[0m\n\n";
	}
}
