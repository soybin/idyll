#pragma once

#include "math.h"

#include <vector>

class region {
private:
	int WIDTH;
	int HEIGHT;
	int MAX_STEP;
	float FOV;
	float RENDER_THRESHOLD;
	float xcoord;
	float ycoord;
	math::vec3 ori;
	math::vec3 col;
	std::vector<char> s;
public:
	region(int width, int height);
	~region();

	void setXCoord(float v);
	void setYCoord(float v);

	const char* getSeed();

	math::vec3 render();
};
