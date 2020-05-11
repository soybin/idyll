#include "seed.h"

// mersennes' twister prng algo initialized with random device
// seed

seed::seed() : rng(dev()) {
}

seed::~seed() {
}

int seed::i(int min, int max) {
	std::uniform_int_distribution<int> dice(min, max);
	return dice(rng);
}

double seed::d(double min, double max) {
	std::uniform_real_distribution<double> dice(min, max);
	return dice(rng);
}
