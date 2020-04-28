#include "region.h"
#include "seed.h"

#define RENDER_THRESHOLD 0.001f
#define MAX_STEP 100

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
}

region::~region() {
}

void region::setXCoord(float xcoord) {
	this->xcoord = xcoord;
}

void region::setYCoord(float ycoord) {
	this->ycoord = ycoord;
}

math::vec3 region::render() {
	// get ray direction relative to the pixel
	// being rendered coordinates'
	math::vec3 dir = math::calcRayDir(math::vec3(), xcoord, ycoord, WIDTH, HEIGHT, f->fov);
	// absolute distance from point of view to
	// the point of intersection with the fractal
	float absDist = 0.0f;
	int step = 0;
	for (; step < MAX_STEP; ++step) {
		math::vec3 point = f->cam + (dir * absDist);
		float relDist = math::de::main(point, f->rot, f->shift, 1.0f, f->iter);
		absDist += relDist;
		if (relDist < RENDER_THRESHOLD) break;
	}
	float val = 255.0f * (1.0f - (float)step / (float)MAX_STEP);
	if (!val) return math::vec3(200.0f, 175.0f, 175.0f);
	return math::vec3(val);
}
