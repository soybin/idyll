#pragma once

#include <string>
#include <fstream>

namespace config {
	extern int getInt(std::string name);
	extern void reset();
}
