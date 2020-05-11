#pragma once

#include "math.h"
#include "seed.h"
#include <vector>
#include <string>

// this structure constitutes the possible operations 
// applied to each iteration of the fractal
struct operation {
	// R -> rotation
	// S -> shift
	// F -> fold
	// mapped to 1, 2, 3
	int action;

	// X, Y or Z in the case of rotation and shift
	// mapped to 1, 2, 3
	int specification;

	// whatever value to apply
	double value;
};

class fractal {
private:
	// fractal's variables. randomly set at runtime. stored at the
	// seed
	int iterations;
	double scale;
	double rz;
	double rx;
	math::vec3 color;
	math::vec3 shift;

	// this is the list of operations that will be executed
	// per rendering loop. creating 
	std::vector<operation> operations;

	// seed pointer
	seed* s;

	// this is used when computing color and DE
	void iteratePoint(math::vec3& point);
public:
	math::vec3 gradientTop;
	math::vec3 gradientBottom;

	fractal(seed* s);
	~fractal();

	// main distance estimator
	double de(math::vec3 point);
	double calculateShadow(math::ray r);
	math::vec3 calculateColor(math::vec3 point);
	math::vec3 calculateNormal(math::vec3 point);
};
