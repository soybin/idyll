#pragma once

#include "math.h"
#include "seed.h"
#include <vector>
#include <string>

// this structure constitutes the possible operations 
// applied to each iteration of the fractal
struct operation {
	// true for rotation, false for fold
	bool isFold;

	// X, Y or Z in the case of rotation
	// M, S, P for Menger and Plane folds
	char specification;

	// in case of a rotation
	double value;
};

class fractal {
private:
	// fractal's variables. randomly set at runtime. stored at the
	// seed
	int iterations;
	double zr;
	double xr;
	double zrs;
	double zrc;
	double xrs;
	double xrc;
	double deSize;
	double shadowSoftness;
	math::vec3 color;
	math::vec3 shift;

	// this is the array of operations that will be executed
	// per fractal iteration
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
