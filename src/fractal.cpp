#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal(seed* s) : s(s) {
	iterations = 7;
	scale = 1.0;
	gradientTop = math::vec3(0.9, 0.2, 0.3);
	gradientBottom = math::vec3(0.1, 0.1, 0.9);
	rz = -0.14;
	rx = -0.23;
	shift.x = -0.53;
	shift.y = -0.01;
	shift.z = -0.21;
	color.x = 0.87;
	color.y = 0.0;
	color.z = 0.99;
}

fractal::~fractal() {
}

void fractal::iteratePoint(math::vec3& point) {
	point = math::absolute(point);
	math::rotation::z(point, rz);
	math::fold::menger(point);
	math::rotation::x(point, rx);
	point *= scale;
	point += shift;
}

double fractal::de(math::vec3 point) {
	for (int i = 0; i < iterations; ++i) {
		iteratePoint(point);
	}
	return math::de::box(point, math::vec3(1.0));
}

// shadowing technique. 
// explained in detal at inigo quilez's blog:
// https://iquilezles.org/www/articles/rmshadows/rmshadows.htm
double fractal::calculateShadow(math::ray r) {
	double res = 0.0;
	double tmax = 12.0;
	double t = 0.001;

	for(int i=0; i<80; i++ ) {
		double h = de(r.origin + r.direction * t);
		if(h < 0.0 || t > tmax) break;
		t += h;
	}

	if(t > tmax) res = 1.0;

	return res;
}

math::vec3 fractal::calculateColor(math::vec3 point) {
	math::vec3 orbit(0.0);
	for (int i = 0; i < iterations; ++i) {
		// do the fractal transformation of the point
		iteratePoint(point);
		// orbit trap coloring
		math::vec3 pc = point * color;
		orbit = math::vec3(std::max(orbit.x, pc.x), std::max(orbit.y, pc.y), std::max(orbit.z, pc.z));
	}
	return orbit;
}

// classical forward and central differences technique.
// explained in detal at inigo quilez blog:
// https://www.iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
math::vec3 fractal::calculateNormal(math::vec3 point) {
	double e = 0.0001;
	math::vec3 xyy(e, 0.0, 0.0);
	math::vec3 yxy(0.0, e, 0.0);
	math::vec3 yyx(0.0, e, 0.0);
	return math::normalize(math::vec3(
				de(point + xyy) - de(point - xyy),
				de(point + yxy) - de(point - yxy),
				de(point + yyx) - de(point - yyx)
				));
}
