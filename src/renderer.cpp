#include "renderer.h"
#include "seed.h"
#include "config.h"

#include <iostream>

const int MAX_STEP = 128;
const double MAX_DIST = 16.0;
const double MIN_DIST = 1e-4;
const double PI = 3.14159265358979;

math::vec3 renderer::calculateRayDirection(double xcoord, double ycoord) {
	//
	// find direction vector
	//
	double x = xcoord - WIDTH / 2.0;
	double y = ycoord - HEIGHT / 2.0;
	double z = HEIGHT / std::tan(FOV * PI / 180.0 / 2.0);
	math::vec3 a = math::vec3(x, y, -z);

	//
	// apply rotations
	//
	a = math::vec3(
		RMx[0].x * a.x + RMx[1].x * a.y + RMx[2].x * a.z,
		RMx[0].y * a.x + RMx[1].y * a.y + RMx[2].y * a.z,
		RMx[0].z * a.x + RMx[1].z * a.y + RMx[2].z * a.z
	);
	a = math::vec3(
		RMy[0].x * a.x + RMy[1].x * a.y + RMy[2].x * a.z,
		RMy[0].y * a.x + RMy[1].y * a.y + RMy[2].y * a.z,
		RMy[0].z * a.x + RMy[1].z * a.y + RMy[2].z * a.z);

	return math::normalize(a);
}

math::vec3 renderer::renderSky(double y, double x) {
	double yy = y / HEIGHT;
	double xx = x / WIDTH;
	math::vec3 g1(0.8, 0.4, 0.4), g2(0.9, 0.2, 0.56);
	return g1 * yy + g2 * (1.0 - yy);
}

renderer::renderer(int WIDTH, int HEIGHT, seed* s, fractal* f) 	: WIDTH(WIDTH), HEIGHT(HEIGHT), s(s), f(f) {

	skyColor = math::vec3(0.8, 0.7, 0.9);
	lightColor = math::vec3(1.0, 1.0, 1.0);

	// get sample value from config file
	SAMPLES = config::getInt("samples");

	// random field of view
	FOV = s->i(60, 60);

	// random positioning of the camera along the surface of a
	// sphere with a certain radius
	double radius = 0.0;
	double dist = s->i(3, 12);
	for (; f->de(math::vec3(0.0, 0.0, radius)) < dist; ) {
		++radius;
	}

	math::vec3 dir(s->d(-1.0, 1.0), s->d(-1.0, 1.0), s->d(-1.0, 1.0));
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

	// determine light direction partially based on camera position
	double cv = 5.0;
	lightDirection = math::normalize(cameraPosition + math::vec3(s->d(-cv, cv), s->d(-cv, cv), s->d(-cv, cv)));
	lightDirection.y = s->i(0, 1) ? s->d(.5, 1.0) : s->d(-1.0, -.5);

	lightDirection = math::vec3(0.0, 1.0, 0.0);

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

	// add some distortion to the rotation
	ry += s->d(-5.0, 5.0);
}

// raymarch
double renderer::march(math::ray r) {
	double res = -1.0;
	double t = 0.01;
	for (int i = 0; i < MAX_STEP; ++i) {
		double h = f->de(r.origin + r.direction * t);
		if (h < 0.0001 || t > MAX_DIST) {
			break;
		}
		t += h;
	}
	if (t < MAX_DIST) res = t;
	return res;
}

// diffuse reflection without tangent thanks to Edd Biddulph:
// http://www.amietia.com/lambertnotangent.html
math::vec3 renderer::lambertReflection(math::vec3 normal) {
	double a = s->d(0.0, 1.0);
	double b = s->d(0.0, 1.0);

	// not an arbitrary number, it's pi * 2
	double theta = 6.283185 * a;
	b = 2.0 * b - 1.0;
	double bsqrt = std::sqrt(1.0 - b * b);
	math::vec3 point(bsqrt * cos(theta), bsqrt * sin(theta), b);
	return math::normalize(normal + point);
}

//
// path tracing
//
math::vec3 renderer::pathTrace(math::ray r, int levels) {
	math::vec3 colorBounce(1.0);
	math::vec3 colorAccumulated(0.0);

	double fdist = 0.0;
	for (int i = 0; i < levels; ++i) {
		//
		// get distance from fractal marching the ray's direction
		//
		double distance = march(r);
		if (distance < 0.0) {
			if (i == 0) {
				return math::vec3(-1.0);
			}
			break;
		}

		if (i == 0) {
			fdist = distance;
		}

		//
		// get current position and normal
		//
		math::vec3 point = r.origin + r.direction * distance;
		math::vec3 normal = f->calculateNormal(point);	

		//
		// get fractal surface color
		//
		colorBounce *= math::vec3(1.4, 1.2, 1.3) * 0.4;//f->calculateColor(point) * 0.4;

		//
		// apply lighting
		//
		math::vec3 color(0.0);
		// directional light
		double dl = std::max(0.0, math::dot(lightDirection, normal));
		double dlShadow = 1.0;
		if (dl > 0.0) {
			dlShadow = f->calculateShadow({r.origin + r.direction * 0.0001, lightDirection});
		}
		color += lightColor * dl * dlShadow;
		// sky dome
		math::vec3 pSky = lambertReflection(normal);
		color += skyColor * f->calculateShadow({r.origin + r.direction * 0.0001, pSky});

		//
		// add to color
		//
		colorAccumulated += colorBounce * color;

		//
		// bounce ray
		//
		r.origin = point;
		r.direction = lambertReflection(normal);
	}

	//
	// scaling / grading
	//
	double ff = std::exp(-0.01 * fdist * fdist);
	colorAccumulated *= ff;
	colorAccumulated += math::vec3(0.9, 1.0, 1.0) * (1.0 - ff) *0.05;

	return colorAccumulated;
}

renderer::~renderer() {
}

math::vec3 renderer::render(double y, double x) {

	// nice variables
	double shutterAperture = 0.6;
  double fov = 2.5;
	double focusDistance = 1.3;
  double blurAmount = 0.0015;
	int numLevels = 5;

	// get ray direction relative to the pixel being rendered
	// coordinates and rotate it
	math::vec3 dir = calculateRayDirection(x, y);
	math::vec3 color(0.0);
	for (int i = 0; i < SAMPLES; ++i) {
		math::ray r(cameraPosition, dir);
		math::vec3 c = pathTrace(r, numLevels);
		if (c == math::vec3(-1.0)) {
			color += renderSky(y, x);
		} else {
			color += c;
		}
	}
	//get average of paths colors
	color /= (double)SAMPLES;

	// apply gamma correction
	color.x = pow(color.x, 0.45);
	color.y = pow(color.y, 0.45);
	color.z = pow(color.z, 0.45);

	return color * 255.0;
}
