// in order to determine the radius of the orbit at which the
// camera should render, we should compute the distance from
// the origin to anywhere in the fractal.
//
// to then obtain a random 3d position across the surface
// of a sphere with radius n, we need to generate two random
// numbers in the range [-n, n], which represent a pair of
// coordinates in two of the three axis to then determine the
// magnitude of the third axis.
//
// this will ensure that we are not inside the fractal nor
// too far away.

#pragma once

#include "math.h"
#include <vector>
#include <string>

// this structure constitutes the list of operations applied
// to each iteration of the fractal.
//
struct operation {
	// R -> rotation
	// S -> shift
	// F -> fold
	char action;

	// X, Y or Z in the case of rotation and shift
	char specification;

	// whatever value to apply
	float value;
};

class fractal {
private:
	// fractal's variables. randomly set at runtime
	int iterations;
	float scale;
	math::vec3 color;
	math::vec3 rotation;
	math::vec3 shift;
	// this is the list of operations that will be executed
	// per rendering loop. creating 
	std::vector<operation> operations;

	// this is used when computing color and DE
	math::vec3 iteratePoint(vec3& point);
public:
	fractal();
	~fractal();

	// main distance estimator
	float de(math::vec3 point);
	float calculateShadow(math::vec3 point, math::vec3 lightDirection);
	math::vec3 calculateColor(math::vec3 point, math::vec3 s1, math::vec3 s2, float radius);
	math::vec3 calculateNormal(math::vec3 point, float radius);
};
