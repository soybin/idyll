/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#include "gui.h"

#include <thread>
#include <cstdlib>

namespace gui {
	bool setup() {
		//
		// clear console.
		// os dependant
		//
#ifdef WINDOWS
		std::system("cls");
#else
		std::system("clear");
#endif

		//
		// draw presentation
		//
		const char* idyll[] = {
			"\033[1;36m",
			"                  /           /  /\n",
			"                /           /  /  \n",
			"     o  ______/           /  /    \n",
			"   /  /     /  /     /  /  /      \n",
			" /  /     /  /     /  /  /        \n",
			"(__(____/(__(____/(__(__(__ o     \n",
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

		double lockedValue = *curV;
		do {
			lockedValue = *curV;
			// width of logo is 34 characters. two of which
			// will be used for the extremes of the bar.
			// terminal colors are represented with expressions
			// like "\033[x;xx"
			std::cout << "\r\033[1;36m[";
			int quantity = 1 + 32 * lockedValue / tarV;
			for (int i = 0; i < 32; ++i) {
				std::cout << (i < quantity ? "\033[1;36m=" : "\033[0m-");
			}
			std::cout << "\033[1;36m]" << std::flush;
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		} while (lockedValue < tarV);
		std::cout << "\033[0;36m\n";
	}
}
