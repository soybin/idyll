#pragma once

#include <string>
#include <fstream>

namespace config {
	extern int getInt(std::string name);
	extern void setSeed(std::string s);
	extern std::string getSeed();
	extern void reset();
}
