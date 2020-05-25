#include "renderer.h"
#include "seed.h"
#include "config.h"

#include <iostream>

const double MAX_DIST = 1024.0;
const double MIN_DIST = 1e-5;
const double PI = 3.14159265358979;

void renderer::updateRotationMatrix() {
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
	return (f->gradientTop * yy + f->gradientBottom * (1.0 - yy));
}

renderer::renderer(int WIDTH, int HEIGHT, seed* s, fractal* f) 	: WIDTH(WIDTH), HEIGHT(HEIGHT), s(s), f(f) {

	// get field of view from config file
	FOV = config::getInt("fov");

	// get sample value from config file
	SAMPLES = config::getInt("samples");

	// get surface bounces per ray
	BOUNCES = config::getInt("bounces");

	// set lower bound for randomness in sky noise
	SKY_NOISE = std::min(std::max(0.8, SAMPLES / 8.0), 1.0);

	// get random glossiness chance and glossiness amount from the
	// region's seed.
	// this represents the probability for a ray to be reflected
	// as glossy instead of as diffuse, and with which intensity
	// it will do so
	GLOSSINESS_CHANCE = s->values["GLOSSINESS_CHANCE"];
	GLOSSINESS_AMOUNT = s->values["GLOSSINESS_AMOUNT"];

	// random positioning of the camera along the surface of a
	// sphere with a certain radius
	math::vec3 dir;
	dir.x = s->values["xcameraDirection"];
	dir.y = s->values["ycameraDirection"];
	dir.z = s->values["zcameraDirection"];
	double distance = s->values["cameraDistance"];
	for (double radius = 0.0; radius < MAX_DIST; ) {
		cameraPosition = dir * radius;
		updateRotationMatrix();
		++radius;
		if (f->de(cameraPosition) < distance) continue;
		bool hit = false;
		for (int i = 0; i < 64; ++i) {
			double y = HEIGHT * s->d(0.0, 1.0);
			double x = WIDTH * s->d(0.0, 1.0);
			double d = march({cameraPosition, calculateRayDirection(x, y)});
			if (d != -1.0) {
				hit = true;
				break;
			}
		}
		if (hit) {
			break;
		}
	}

	// get directional light direction
	lightDirection.x = s->values["xlightDirection"];
	lightDirection.y = s->values["ylightDirection"];
	lightDirection.z = s->values["zlightDirection"];

	// get directional light color
	lightColor.x = s->values["xlightColor"];
	lightColor.y = s->values["ylightColor"];
	lightColor.z = s->values["zlightColor"];
	
	// get sky color
	skyColor.x = s->values["xskyColor"];
	skyColor.y = s->values["yskyColor"];
	skyColor.z = s->values["zskyColor"];
}

// raymarch
double renderer::march(math::ray r) {
	double t = MIN_DIST;
	for (; t < MAX_DIST; ) {
		double h = f->de(r.origin + r.direction * t);
		if (h < MIN_DIST) {
			break;
		}
		t += h;
	}
	if (t < MAX_DIST) return t;
	return -1.0;
}

math::vec3 renderer::brdf(math::vec3 direction, math::vec3 normal) {
	if (s->d(0.0, 1.0) > GLOSSINESS_CHANCE) {
		//
		// diffuse reflection without tangent thanks to Edd 
		// Biddulph:
		// http://www.amietia.com/lambertnotangent.html
		//
		double a = s->d(0.0, 1.0);
		double b = s->d(0.0, 1.0);
		// not an arbitrary number, it's pi * 2
		double theta = 6.283185 * a;
		b = 2.0 * b - 1.0;
		double bsqrt = std::sqrt(1.0 - b * b);
		math::vec3 point(bsqrt * cos(theta), bsqrt * sin(theta), b);
		return math::normalize(normal + point);
	} else {
		//
		// glossy reflection
		//
		math::vec3 reflection = math::reflect(direction, normal);
		double a = PI;
		double b = 2 * PI;
		math::vec3 uniformVector(std::sin(b) * std::sin(a), std::cos(b) * std::sin(a), std::cos(a));
		return math::normalize(reflection + uniformVector) * GLOSSINESS_AMOUNT;
	}

}

renderer::~renderer() {
}

math::vec3 renderer::render(double y, double x) {

	// get ray direction relative to the pixel being rendered
	// coordinates and rotate it
	math::vec3 dir = calculateRayDirection(x, y);
	math::vec3 color(0.0);

	//
	// render same pixel multiple times
	//
	for (int i = 0; i < SAMPLES; ++i) {

		//
		// data per sample
		//
		math::ray r(cameraPosition, dir);
		math::vec3 colorLeft(1.0);
		math::vec3 colorAccumulated(0.0);

		//
		// path tracing
		//
		double fdist = 0.0;
		for (int i = 0; i < BOUNCES; ++i) {

			//
			// get distance from fractal marching the ray's direction
			//
			double distance = march(r);
			if (distance == -1.0) {
				if (i == 0) {
					colorAccumulated = renderSky(y, x);
				}
				break;
			}

			//
			// record initial distance
			//
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
			math::vec3 colorAtPoint = f->calculateColor(point);

			math::vec3 colorLighting(0.0);
			//
			// directional light
			//
			double dl = std::max(0.0, math::dot(lightDirection, normal));
			double dlShadow = 1.0;
			if (dl > 0.0) {
				dlShadow = f->calculateShadow({r.origin + r.direction * MIN_DIST, lightDirection});
			}
			colorLighting += lightColor * dl * dlShadow;

			//
			// sky light
			//
			double skyShadow = f->calculateShadow({r.origin + r.direction * MIN_DIST, brdf(r.direction, normal)});
			colorLighting += skyColor * skyShadow;

			//
			// add bounce color to sample color
			//
			colorLeft *= colorAtPoint;
			colorAccumulated += colorLeft * colorLighting;

			//
			// bounce ray
			//
			r.origin = point;
			r.direction = brdf(r.direction, normal);
		}

		//
		// scaling / grading
		//
		double ff = std::exp(-0.01 * fdist * fdist);
		colorAccumulated *= ff;
		colorAccumulated += math::vec3(0.9, 1.0, 1.0) * (1.0 - ff) *0.05;

		//
		// add clamped sample color to average color
		//
		color += math::clamp(colorAccumulated, 0.0, 1.0);
	}
	//get average of paths colors
	color /= (double)SAMPLES;

	// apply gamma correction
	color.x = pow(color.x, 0.45);
	color.y = pow(color.y, 0.45);
	color.z = pow(color.z, 0.45);

	return color * 255.0;
}
