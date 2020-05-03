#include "region.h"
#include "seed.h"

#define MIN_DIST 0.01f
#define MAX_STEP 150

math::vec3 region::march(math::vec3 point, math::vec3 direction) {
	float totalDistance = 0.0f;
	for (int step; step < MAX_STEP; ++step) {
		point = cameraPosition + (direction * totalDistance);
		float relativeDistance = f->de(point);
		totalDistance += relativeDistance;
		if (relativeDistance < MIN_DIST) {
			return point;
		}
	}
	return math::vec3(-1.0f);
}

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
	lightDirection = math::vec3(0.2f, 0.2f, 0.2f);
	FOV = seed::i(60, 60);
	// set camera position
	for (; f->de(cameraPosition) < 2.5f; ) {
		cameraPosition.z += 0.1f;
	}
}

region::~region() {
}

math::vec3 region::render(float y, float x) {
	// get ray direction relative to the pixel being rendered
	// coordinates
	math::vec3 dir = math::calcRayDir(math::vec3(), x, y, WIDTH, HEIGHT, FOV);

	// get the point of the intersection between the fractal and
	// the camera ray
	//math::vec3 point = march(cameraPosition, dir);

	math::vec3 point(0.0f);
	float totalDist = 0.0f;
	int steps = 0;
	for (; steps < MAX_STEP; ++steps) {
		point = cameraPosition + dir * totalDist;
		float dist = f->de(point);
		totalDist += dist;
		if (dist < MIN_DIST) {
			break;
		}
	}
	// backgroud color. nothing has been hit
	if (steps == MAX_STEP) {
		return math::vec3(0.0f);
	}

	// fractal hit.
	// compute normal of the surface
	math::vec3 normal = f->calculateNormal(point, MIN_DIST);
	
	//point -= normal * f->de(point);
	// find closest surface point. otherwise bad happens
	//math::vec3 campos = cameraPosition - normal * f->de(point);
	math::vec3 s1 = math::normalize(dir * normal);
	float shadow = f->calculateShadow(point, lightDirection);
	
	return f->calculateColor(point, s1, s1 * normal, 0.1f) * shadow * 255.0f;
}
