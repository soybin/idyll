#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class renderer {
private:
	int WIDTH;
	int HEIGHT;
	int FOV;
	int SAMPLES;
	double SKY_NOISE;
	math::vec3 cameraPosition;
	math::vec3 skyColor;
	math::vec3 lightDirection;
	math::vec3 lightColor;
	std::vector<math::vec3> RMx;
	std::vector<math::vec3> RMy;

	// object pointers
	fractal* f;
	seed* s;

	// get ray direction based on screen coordinates and apply
	// yaw and pitch rotation to it
	math::vec3 calculateRayDirection(double y, double x);

	// render functions
	double march(math::ray r);
	math::vec3 lambertReflection(math::vec3 normal);
	math::vec3 pathTrace(math::ray r, int levelsLeft);
	math::vec3 renderSky(math::ray r, double y, double x);

public:
	renderer(int width, int height, seed* s, fractal* f);
	~renderer();

	math::vec3 render(double y, double x);
};
