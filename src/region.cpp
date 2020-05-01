#include "region.h"
#include "seed.h"

#define RENDER_THRESHOLD 0.01f
#define MAX_STEP 100

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
}

region::~region() {
}

math::vec3 region::render(float y, float x) {
	// get ray direction relative to the pixel
	// being rendered coordinates'
	math::vec3 dir = math::calcRayDir(math::vec3(), x, y, WIDTH, HEIGHT, f->fov);
	math::vec3 point(0.0f);
	// absolute distance from point of view to
	// the point of intersection with the fractal
	float absDist = 0.0f;
	int step = 0;
	for (; step < MAX_STEP; ++step) {
		point = f->cam + (dir * absDist);
		float relDist = math::de::main(point, f->rot, f->shift, 1.0f, f->iter);
		absDist += relDist;
		if (relDist < RENDER_THRESHOLD) break;
	}
	if (step == MAX_STEP) return vec3(0.0f);
	vec3 normal = math::calcRayNormal();
	vec3 s1 = (dir * normal).normalize();
	return math::color::smooth(point, s1, s2, math::vec3(-0.42f, 0.23f, 0.19f), f->rot, f->shift, 1.0f, f->iter) * 255.0f;
}
