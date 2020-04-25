#include "region.h"
#include "seed.h"

region::region(int WIDTH, int HEIGHT) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT) {
	MAX_STEP = 50;
	FOV = 45.0f;
	RENDER_THRESHOLD = 0.01f;
	ori = math::vec3();
	col = math::vec3();
	s = seed::get({
			{'A', 'E'},
			{'F', 'Z'},
			{'A', 'a'},
			{'M', 'O'},
			{'A', 'F'}
	});
}

region::~region() {
}

void region::setXCoord(float xcoord) { 
	this->xcoord = xcoord;
}

void region::setYCoord(float ycoord) {
	this->ycoord = ycoord;
}

const char* region::getSeed() {
	return &s[0];
}

math::vec3 region::render() {
	// get ray direction relative to the pixel
	// being rendered coordinates'
	math::vec3 dir = math::calcRayDir(xcoord, ycoord, WIDTH, HEIGHT, FOV);
	// absolute distance from point of view to
	// the point of intersection with the fractal
	float absDist = 0.0f;
	int step = 0;
	for (; step < MAX_STEP; ++step) {
		math::vec3 point = ori + (dir * absDist);
		float relDist = math::de::sphere(point);
		absDist += relDist;
		if (relDist < RENDER_THRESHOLD) break;
	}
	return math::vec3(255.0f * (1.0 - float(step) / float(MAX_STEP)));
}

