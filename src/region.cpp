#include "region.h"
#include "seed.h"

#include <iostream>

#define MAX_DIST 50.0f
#define MIN_DIST 0.001f
#define MAX_STEP 1000
#define OCCLUSION_STRENGTH 0.3f
#define SHADOW_STRENGTH 0.2f

#define PI 3.14159265358979

math::vec3 region::calculateRayDirection(float xcoord, float ycoord) {
	// find direction vector
	float x = xcoord - WIDTH / 2.0f;
	float y = ycoord - HEIGHT / 2.0f;
	float z = HEIGHT / std::tan(FOV * PI / 180.0f / 2.0f);
	math::vec3 a = math::normalize(math::vec3(x, y, -z));
	return a;
}

math::vec3 region::march(math::vec3 direction, int& step) {
	math::vec3 point(0.0f);
	float totalDistance = 0.0f;
	for (step = 0; step < MAX_STEP; ++step) {
		point = cameraPosition + (direction * totalDistance);
		float relativeDistance = f->de(point);
		if (relativeDistance < MIN_DIST) {
			return point;
		} else if (totalDistance > MAX_DIST) {
			break;
		}
		totalDistance += relativeDistance;
	}
	return math::vec3(-1.0f);
}

math::vec3 region::renderBg(float y, float x) {
	return math::vec3(0.0f);
}

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
	lightDirection = math::vec3(0.2f, 0.2f, 0.2f);

	// camera thingies
	
	// random field of view
	FOV = seed::i(60, 60);

	// random positioning of the camera along the surface of a sphere
	// with a certain radius
	int radius = 30;
	math::vec3 dir(seed::f(-1.0f, 1.0f), seed::f(-1.0f, 1.0f), seed::f(-1.0f, 1.0f));
	for (float totalDistance = 0.0f;;) {
		cameraPosition = dir * totalDistance;
		// it's important to change the sign or get the absolute value
		// of the distance since we are positioned inside the sphere
		float dist = -math::de::sphere(cameraPosition, radius);
		if (dist < MIN_DIST) {
			break;
		}
		totalDistance += dist;
	}

	cameraPosition = math::vec3(0.0f, 0.0f, 30.0f);
	// determine camera's rotation so that it always looks towards
	// the center of the scene regardless of its position
	//
	//    roll			 pitch			yaw
	float rz = 0.0f, ry = 0.0f, rx = 0.0f;

	// convert to radians
	rz *= PI / 180.0f;
	ry *= PI / 180.0f;
	rx *= PI / 180.0f;
	// trig
	float sinz = std::sin(rz), cosz = std::cos(rz);
	float siny = std::sin(ry), cosy = std::cos(ry);
	float sinx = std::sin(rx), cosx = std::cos(rx);

	// rotation matrix. 
	// there's no explicit matrix implementation because there's
	// only one use case in the domain of this program. insted,
	// i implemented it by having different vec3 arrays with 3
	// vec3 each. there's three of them, each one serves for 
	// rotations on a different axis.
	// basic rotations implementation.
	// https://en.wikipedia.org/wiki/Rotation_matrix
	RM = {
		math::vec3(cosz*cosy, cosz*siny*sinx-sinz*cosx, cosz*siny*cosx+sinz*sinx),
		math::vec3(sinz*cosy, sinz*siny*sinx+cosz*cosx, sinz*siny*cosx-cosz*sinx),
		math::vec3((-siny), cosy*sinx, cosy*cosx)
	};
}

region::~region() {
}

math::vec3 region::render(float y, float x) {
	// get ray direction relative to the pixel being rendered
	// coordinates
	math::vec3 preDir = calculateRayDirection(x, y);
	//math::rotateVector(dir, math::vec3(0.0f, 0.0f, 0.0f));
	math::vec3 dir(
			RM[0].x*preDir.x + RM[1].x*preDir.y + RM[2].x*preDir.z,
			RM[0].y*preDir.x + RM[1].y*preDir.y + RM[2].y*preDir.z,
			RM[0].z*preDir.x + RM[1].z*preDir.y + RM[2].z*preDir.z
	);

	// get the point of the intersection between the fractal and
	// the camera ray
	int step;
	math::vec3 point = march(dir, step);

	// check if the ray didn't hit anything and if so, calculate
	// gradient background based on pixel position
	if (point == math::vec3(-1.0f)) {
		return math::vec3(0.0f);
	}

	math::vec3 ret(0.0f);

	// compute normal of the surface
	math::vec3 normal = f->calculateNormal(point, MIN_DIST / 2.0f);

	// get samples to calculate colors
	math::vec3 s1 = math::normalize(math::cross(normal, dir));
	math::vec3 s2 = math::cross(s1, normal);

	// fractal coloring
	math::vec3 color = math::normalize(f->calculateColor(point, s1, s2, MIN_DIST / 2.0f));

	// shadowing
	float shadow = f->calculateShadow(point, lightDirection);
	shadow = std::max(shadow, 1.0f - SHADOW_STRENGTH);

	ret += color * shadow;

	// ambient occlusion
	float occlusion = 1.0 / (1.0 + step * 0.008f);
	ret += math::vec3(OCCLUSION_STRENGTH) * (1.0f - occlusion);

	return ret * 255.0f;
}
