#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class region {
private:
	int WIDTH;
	int HEIGHT;
	int FOV;
	float shadowSmoothness;
	math::vec3 cameraPosition;
	std::vector<math::vec3> xRM;
	std::vector<math::vec3> yRM;
	std::vector<math::vec3> zRM;
	std::vector<math::vec3> RM;
	math::vec3 lightDirection;
	fractal* f;

	math::vec3 calculateRayDirection(float y, float x);

	// marching!!!!
	math::vec3 march(math::vec3 direction, int& step);

	// in case the marched ray doesn't hit the fractal we render
	// the background based on the pixel coordinates
	math::vec3 renderBg(float y, float x);
public:

	region(int width, int height, fractal* f);
	~region();

	math::vec3 render(float y, float x);
};
