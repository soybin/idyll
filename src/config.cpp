/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#include "config.h"

#include <iostream>

namespace config {
	int getInt(std::string name) {
		// file handle
		std::ifstream file("config.txt");

		// create file in case it doesn't exist
		if (!file.good()) {
			std::cout << "[-] Couldn't find 'config.txt' file.\n";
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
					std::cout << "[-] Invalid value for variable '" << name << "' in config.txt file.\n";
					file.close();
					reset();
					return getInt(name);
				}
			}
		}

		std::cout << "[-] Variable '" << name << "' was not found in 'config.txt' file.\n";
		file.close();
		reset();
		return getInt(name);
	}

	void reset() {
		std::cout << "[+] Resetting config.txt file to standard values.\n";
		std::ofstream file("config.txt");
		file << "#======== o u t p u t    f i l e ========#\n";
		file << "\n";
		file << "# resolution in pixels #\n";
		file << "width 1920\n";
		file << "height 1080\n";
		file << "\n";
		file << "# set to one to get a png file #\n";
		file << "# set to zero to get a ppm file #\n";
		file << "png 1\n";
		file << "\n";
		file << "#======== r e n d e r i n g ========#\n";
		file << "\n";
		file << "# number of samples #\n";
		file << "# more samples equals less noise #\n";
		file << "samples 4\n";
		file << "\n";
		file << "# camera field of view in degrees #\n";
		file << "fov 45\n";
		file << "\n";
		file << "# surface bounces per path traced ray #\n";
		file << "# this value increases quality and computation time #\n";
		file << "bounces 2\n";
		file << "\n";
		file << "#======== c p u ========#\n";
		file << "\n";
		file << "# idyll is multithreaded by default #\n";
		file << "# you can adjust the number of threads here #\n";
		file << "threads 8\n";
		file << "\n";
	}
}
