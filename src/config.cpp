#include "config.h"
#include <iostream>

namespace config {
	int getInt(std::string name) {
		std::ifstream file("config.txt");

		// create file in case it doesn't exist
		if (!file.good()) {
			config::reset();
			file = std::ifstream("config.txt");
		}

		for (std::string line; std::getline(file, line); ) {
			size_t found = line.find(name);
			if (found != std::string::npos) {
				try {
					return std::stoi(line.substr(found + name.size()));	
				} catch (...) {
					std::cout << "[-] invalid config.txt file.\n";
					return -1;
				}
			}
		}
		reset();
		return -1;
	}

	void reset() {
		std::ofstream file("config.txt");
		file << "width 1920\nheight 1080\n";
	}
}
