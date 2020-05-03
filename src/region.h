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
	math::vec3 lightDirection;
	fractal* f;

	math::vec3 march(math::vec3 point, math::vec3 direction);
public:

	region(int width, int height, fractal* f);
	~region();

	math::vec3 render(float y, float x);
};
