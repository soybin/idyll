#pragma once

#include "math.h"

#include <string>
#include <vector>
#include <random>

class seed {
private:
	std::random_device dev;
	std::mt19937 rng;

public:
	seed();
	~seed();

	// get random integer in range between min and max
	int i(int min, int max);
	// get random double in range between min and max
	double d(double min, double max);
};
