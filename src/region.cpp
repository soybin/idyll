#include "region.h"
#include "seed.h"

#include <iostream>

#define MAX_DIST 100.0
#define MIN_DIST 0.001
#define MAX_STEP 1000
#define OCCLUSION_STRENGTH 0.4
#define SHADOW_STRENGTH 0.2

const double PI = 3.14159265358979;

math::vec3 region::calculateRayDirection(double xcoord, double ycoord) {
	// find direction vector
	double x = xcoord - WIDTH / 2.0;
	double y = ycoord - HEIGHT / 2.0;
	double z = HEIGHT / std::tan(FOV * PI / 180.0 / 2.0);
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
	math::vec3 point(0.0);
	double totalDistance = 0.0;
	for (step = 0; step < MAX_STEP; ++step) {
		point = cameraPosition + (direction * totalDistance);
		double relativeDistance = f->de(point);
		if (relativeDistance < MIN_DIST) {
			return point;
		} else if (totalDistance > MAX_DIST) {
			break;
		}
		totalDistance += relativeDistance;
	}
	return math::vec3(-1.0);
}

math::vec3 region::renderBg(double y, double x) {
	return math::vec3(y * 255.0);
}

region::region(int WIDTH, int HEIGHT, fractal* f) 
	: WIDTH(WIDTH), HEIGHT(HEIGHT), f(f) {
	lightDirection = math::vec3(0.2, 0.2, 0.2);

	// random field of view
	FOV = seed::i(45, 45);

	// random positioning of the camera along the surface of a
	// sphere with a certain radius
	double radius = 0.0;
	for (; f->de(math::vec3(0.0, 0.0, radius)) < seed::f(6.0, 18.0); ) {
		radius += seed::f(0.0, 6.0);
	}
	
	math::vec3 dir(seed::f(-1.0, 1.0), seed::f(-1.0, 1.0), seed::f(-1.0, 1.0));
	for (double totalDistance = 0.0f;;) {
		cameraPosition = dir * totalDistance;
		// change sign of the distance because we are inside the
		// sphere
		double dist = -math::de::sphere(cameraPosition, radius);
		if (dist < MIN_DIST) {
			break;
		}
		totalDistance += dist;
	}
	
	// determine camera's rotation so that it always looks
	// towards the center of the scene regardless of its
	// position

	// pitch
	double ry = 0.0;
	// yaw
	double rx = 0.0;
	
	double x = cameraPosition.x;
	double y = cameraPosition.y;
	double z = cameraPosition.z;

	// yaw
	// to calculate the yaw axis rotation we have to think
	// about the intersection of the sphere at y height
	// because the distance towards the center will decrease
	// as we get closer to the poles. i wasted a whole day 
	// because of this
	ry = std::asin(std::abs(z) / std::sqrt(x * x + z * z));
	if (x < 0.0) {
		if (z < 0.0) {
			ry = (PI / 2.0) + ry;
		} else if (z > 0.0) {
			ry = (PI / 2.0) - ry;
		} else {
			ry = 90.0;
		}
	} else if (x > 0.0) {
		if (z < 0.0) {
			ry = -((PI / 2.0) + ry);
		} else if (z > 0.0){
			ry = -((PI / 2.0) - ry);
		} else {
			ry = -(PI / 2.0);
		}
	} else {
		if (z < 0.0) {
			ry = -PI;
		} else if (z > 0.0) {
			ry = 0.0;
		}
	}

	// pitch
	rx = std::asin(y / std::sqrt(x * x + y * y + z * z));

	// trig
	double siny = std::sin(ry);
	double cosy = std::cos(ry);
	double sinx = std::sin(rx);
	double cosx = std::cos(rx);

	// rotation matrix. 
	// there's no explicit matrix implementation because there's
	// only one use case in the domain of this program. i've
	// implemented it by having a static array of vec3s with
	// precomputed values that are multiplied by each ray 
	// direction vector at runtime.
	// https://en.wikipedia.org/wiki/Rotation_matrix
	RMy = {
		math::vec3(cosy, 0.0, siny),
		math::vec3(0.0, 1.0, 0.0),
		math::vec3(-siny, 0.0, cosy)
	};

	RMx = {
		math::vec3(1.0, 0.0, 0.0),
		math::vec3(0.0, cosx, -sinx),
		math::vec3(0.0, sinx, cosx)
	};
}

region::~region() {
}

math::vec3 region::render(double y, double x) {
	// get ray direction relative to the pixel being rendered
	// coordinates and rotate it
	math::vec3 dir = calculateRayDirection(x, y);

	// get the point of the intersection between the fractal and
	// the camera ray
	int step;
	math::vec3 point = march(dir, step);

	// check if the ray didn't hit anything and if so, calculate
	// gradient background based on pixel position
	if (point == math::vec3(-1.0)) {
		return math::vec3(0.0);
	}
	
	math::vec3 ret(0.0);

	// compute normal of the surface
	math::vec3 normal = f->calculateNormal(point, MIN_DIST / 2.0);

	// get samples to calculate colors
	math::vec3 s1 = math::normalize(math::cross(normal, dir));
	math::vec3 s2 = math::cross(s1, normal);

	// fractal coloring
	math::vec3 color = math::normalize(f->calculateColor(point, s1, s2, MIN_DIST / 2.0));

	// shadows
	double shadow = f->calculateShadow(point, lightDirection);
	shadow = std::max(shadow, 1.0 - SHADOW_STRENGTH);

	color = math::normalize(math::vec3(1.0) + (color / 2.0));

	ret += color * shadow;

	// ambient occlusion
	double occlusion = 1.0 / (1.0 + step * 0.008);
	ret += math::vec3(OCCLUSION_STRENGTH) * (1.0 - occlusion);

	return ret * 255.0;
}
