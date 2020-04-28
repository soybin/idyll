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

struct fractal {
	fractal();
	~fractal();

	int fov;
	int iter;
	math::vec3 cam;
	math::vec3 col;
	math::vec3 rot;
	math::vec3 shift;
	// this is the list of operations that will be executed
	// per rendering loop. creating 
	std::vector<char> operations;
};
