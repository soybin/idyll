#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class region {
private:
	fractal* f;
	int WIDTH;
	int HEIGHT;
	float xcoord;
	float ycoord;
public:
	region(int width, int height, fractal* f);
	~region();

	void setXCoord(float v);
	void setYCoord(float v);

	math::vec3 render();
};
