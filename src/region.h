#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class region {
private:
	fractal* f;
	int WIDTH;
	int HEIGHT;
public:
	region(int width, int height, fractal* f);
	~region();

	math::vec3 render(float y, float x);
};
