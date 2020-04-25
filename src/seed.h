#pragma once

#include <string>
#include <vector>
#include <random>

namespace seed {
	extern char randomValue(char min, char max);
	extern std::vector<char> get(const std::vector<std::pair<char, char>>& r);
}
