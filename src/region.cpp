#include "region.h"
#include "seed.h"

#include <iostream>

#define MAX_DIST 100.0f
#define MIN_DIST 0.001f
#define MAX_STEP 1000
#define OCCLUSION_STRENGTH 0.3f
#define SHADOW_STRENGTH 0.2f

const float PI = 3.14159265358979;

math::vec3 region::calculateRayDirection(float xcoord, float ycoord) {
	// find direction vector
	float x = xcoord - WIDTH / 2.0f;
	float y = ycoord - HEIGHT / 2.0f;
	float z = HEIGHT / std::tan(FOV * PI / 180.0f / 2.0f);
	math::vec3 a = math::normalize(math::vec3(x, y, -z));
	// apply rotations
	a = math::vec3(
		RMx[0].x * a.x + RMx[1].x * a.y + RMx[2].x * a.z,
		RMx[0].y * a.x + RMx[1].y * a.y + RMx[2].y * a.z,
		RMx[0].z * a.x + RMx[1].z * a.y + RMx[2].z * a.z
	);
	a = math::vec3(
		RMy[0].x * a.x + RMy[1].x * a.y + RMy[2].x * a.z,
		RMy[0].y * a.x + RMy[1].y * a.y + RMy[2].y * a.z,
		RMy[0].z * a.x + RMy[1].z * a.y + RMy[2].z * a.z
	);

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
	return math::vec3(y * 255.0f);
}

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
	lightDirection = math::vec3(0.2f, 0.2f, 0.2f);

	// random field of view
	FOV = seed::i(45, 45);

	// random positioning of the camera along the surface of a
	// sphere with a certain radius
	int radius = 30;
	
	math::vec3 dir(seed::f(-1.0f, 1.0f), seed::f(-1.0f, 1.0f), seed::f(-1.0f, 1.0f));
	for (float totalDistance = 0.0f;;) {
		cameraPosition = dir * totalDistance;
		// change sign of the distance because we are inside the
		// sphere
		float dist = -math::de::sphere(cameraPosition, radius);
		if (dist < MIN_DIST) {
			break;
		}
		totalDistance += dist;
	}
	
	// determine camera's rotation so that it always looks
	// towards the center of the scene regardless of its
	// position

	// pitch
	float ry = 0.0f;
	// yaw
	float rx = 0.0f;
	
	float x = cameraPosition.x;
	float y = cameraPosition.y;
	float z = cameraPosition.z;

	// yaw
	// to calculate the yaw axis rotation we have to think
	// about the intersection of the sphere at y height
	// because the distance towards the center will decrease
	// as we get closer to the poles. i wasted a whole day 
	// because of this
	ry = std::asin(std::abs(z) / std::sqrt(x * x + z * z));
	if (x < 0.0f) {
		if (z < 0.0f) {
			ry = (PI / 2.0f) + ry;
		} else if (z > 0.0f){
			ry = (PI / 2.0f) - ry;
		} else {
			ry = 90.0f;
		}
	} else if (x > 0.0f) {
		if (z < 0.0f) {
			ry = -((PI / 2.0f) + ry);
		} else if (z > 0.0f){
			ry = -((PI / 2.0f) - ry);
		} else {
			ry = -(PI / 2.0f);
		}
	} else {
		if (z < 0.0f) {
			ry = -PI;
		} else if (z > 0.0f) {
			ry = 0.0f;
		}
	}

	// pitch
	rx = std::asin(y / std::sqrt(x * x + y * y + z * z));

	// trig
	float siny = std::sin(ry);
	float cosy = std::cos(ry);
	float sinx = std::sin(rx);
	float cosx = std::cos(rx);

	// rotation matrix. 
	// there's no explicit matrix implementation because there's
	// only one use case in the domain of this program. i've
	// implemented it by having a static array of vec3s with
	// precomputed values that are multiplied by each ray 
	// direction vector at runtime.
	// https://en.wikipedia.org/wiki/Rotation_matrix
	RMy = {
		math::vec3(cosy, 0, siny),
		math::vec3(0, 1, 0),
		math::vec3(-siny, 0, cosy)
	};

	RMx = {
		math::vec3(1, 0, 0),
		math::vec3(0, cosx, -sinx),
		math::vec3(0, sinx, cosx)
	};
}

region::~region() {
}

math::vec3 region::render(float y, float x) {
	// get ray direction relative to the pixel being rendered
	// coordinates and rotate it
	math::vec3 dir = calculateRayDirection(x, y);

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

	// shadows
	float shadow = f->calculateShadow(point, lightDirection);
	shadow = std::max(shadow, 1.0f - SHADOW_STRENGTH);

	ret += color * shadow;

	// ambient occlusion
	float occlusion = 1.0 / (1.0 + step * 0.008f);
	ret += math::vec3(OCCLUSION_STRENGTH) * (1.0f - occlusion);

	return ret * 255.0f;
}
