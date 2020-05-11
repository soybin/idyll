#include "config.h"
#include <iostream>

namespace config {
	int getInt(std::string name) {
		std::ifstream file("config.txt");

		// create file in case it doesn't exist
		if (!file.good()) {
			std::cout << "[-] couldn't find config.txt file.\n";
			file.close();
			reset();
			return getInt(name);
		}

		for (std::string line; std::getline(file, line); ) {
			if (line[0] == '#') continue;
			size_t found = line.find(name);
			if (found != std::string::npos) {
				try {
					// check to see if there's any non-numeric character
					for (auto& c : line.substr(found + name.size())) {
						if ((c < '0' || c > '9') && c != ' ') {
							throw 1;
						}
					}
					// get int from line substring
					return std::stoi(line.substr(found + name.size()));
				} catch (...) {
					std::cout << "[-] invalid value for variable '" << name << "' in config.txt file.\n";
					file.close();
					reset();
					return getInt(name);
				}
			}
		}

		std::cout << "[-] variable '" << name << "' was not found in config.txt file.\n";
		file.close();
		reset();
		return getInt(name);
	}

	void reset() {
		std::cout << "[+] resetting config.txt file to standard values.\n";
		std::ofstream file("config.txt");
		file << "# output image file variables #\n";
		file << "# you may modify these values #\n";
		file << "width 1920\n";
		file << "height 1080\n";
		file << "\n";
		file << "# number of samples #\n";
		file << "# more samples means less noise #\n";
		file << "samples 4\n";
		file << "# multithread instructions #\n";
		file << "threads 100\n";
		file << "\n";
	}
}
