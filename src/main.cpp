#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define CURRENT_PLATFORM_UNIX
#include <unistd.h>
#include <term.h>
#elif __WIN32
#define CURRENT_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <fstream>
#include "math.h"
#include "seed.h"
#include "region.h"

int main() {
	// clear screen. os dependent
#if defined(CURRENT_PLATFORM_WINDOWS)

	HANDLE hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = { 0, 0 };

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE) {
		printf("couldn't get terminal handle.\n");
		return 0;
	}

	// get number of cells in current terminal
	 if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
		 printf("couldn't retrieve terminal info.\n");
		 return 0;
	 }
	 cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	 // fill terminal with spaces
	 if (!FillConsoleOutputCharacter(hStdOut, (TCHAR)' ', cellCount, homeCoords, &count)) {
		 printf("couldn't fill terminal with spaces.\n");
		 return 0;
	 }

	 // fill terminal with previous attributes and colors
	 if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount, homeCoords, &count)) {
		 printf("couldn't fill terminal with previous attributes.\n");
		 return 0;
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
			 printf("couldn't instantiate nor find terminal.\n");
			 return 0;
		 }
	 }
	 putp(tigetstr("clear"));

#else

	printf("platform not supported at the moment. sorry.\n");
	return 0;

#endif
	// draw presentation
	const char* idyll[10] = {
		"                  /'          /' /' \n",
		"                /'          /' /'   \n",
		"     o  _____,/'          /' /'     \n",
		"   /' /'    /' /'    /  /' /'       \n",
		" /' /'    /' /'    /' /' /'         \n",
		"(__(___,/(__(___,/(__(__(__  o      \n",
		"               /'                   \n",
		"       /     /'                     \n",
		"      (___,/'                       \n",
		"\n ~b  y        s  o  y  b  i  n~\n\n"
	};
	for (int i = 0; i < sizeof(idyll) / sizeof(const char*); ++i) printf("%s", idyll[i]);

	// get desired width and height from user input
	unsigned int width, height;
	printf("width in pixels: ");
	scanf("%d", &width);
	printf("height in pixels: ");
	scanf("%d", &height);

	// initialize output file
	std::ofstream out("out.ppm");
	out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';

	// create seeded region
	region* reg = new region(width, height);
	
	for (float y = (float)height - 0.5f; y > 0.0f; --y) {
		reg->setYCoord(y);
		for (float x = 0.5f; x < width; ++x) {
			reg->setXCoord(x);
			math::vec3 pixel = reg->render();
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}

	// free fractal <3
	delete reg;

	return 0;
}
