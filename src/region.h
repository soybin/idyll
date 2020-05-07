#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class region {
private:
	int WIDTH;
	int HEIGHT;
	int FOV;
	double shadowSmoothness;
	math::vec3 cameraPosition;
	math::vec3 lightDirection;
	std::vector<math::vec3> RMx;
	std::vector<math::vec3> RMy;
	fractal* f;

	// get ray direction based on screen coordinates and apply
	// yaw and pitch rotation to it
	math::vec3 calculateRayDirection(double y, double x);

	// marching!!!!
	math::vec3 march(math::vec3 direction, int& step);
	// in case the marched ray doesn't hit the fractal we render
	// the background based on the pixel coordinates
	math::vec3 renderBg(double y, double x);
public:

	region(int width, int height, fractal* f);
	~region();

	math::vec3 render(double y, double x);
};
