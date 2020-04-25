#include "seed.h"

namespace seed {
	char getValue(char min, char max) {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
		return dist(rng);
	}

	std::vector<char> get(const std::vector<std::pair<char, char>>& r) {
		std::vector<char> ret;
		for (const auto& c : r) {
			ret.push_back(getValue(c.first, c.second));
		}
		return ret;
	}
}
