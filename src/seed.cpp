#include "seed.h"

namespace seed {

	int i(int min, int max) {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
		return dist(rng);
	}

	double f(double min, double max) {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<> dist(min, max);
		return dist(rng);
	}
}
