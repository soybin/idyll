#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal(seed* s) : s(s) {

	//
	// number of fractal iterations
	//
	iterations = s->i(16, 16);
	std::cout << "iterations: " << iterations << std::endl;

	//
	// determine fractal color
	//
	color.x = s->d(0.0, 1.0);
	color.y = s->d(0.0, 1.0);
	color.z = s->d(0.0, 1.0);
	color = math::normalize(color);

	//
	// determine color variation bound
	//
	double cv = s->d(0.0, 1.0);

	//
	// determine top sky gradient color variation
	// 
	gradientTop = math::vec3(s->d(0.0, cv)) + color;
	gradientTop.x += s->d(-cv, cv);
	gradientTop.y += s->d(-cv, cv);
	gradientTop.z += s->d(-cv, cv);
	gradientTop = math::normalize(gradientTop);

	//
	// determine bottom sky gradient color
	//
	gradientBottom = math::vec3(s->d(0.0, cv / 2.0)) + color;
	gradientBottom.x += s->d(-cv, cv);
	gradientBottom.y += s->d(-cv, cv);
	gradientBottom.z += s->d(-cv, cv);
	gradientBottom = math::normalize(gradientBottom);

	//
	// randomly change gradient color positions
	//
	if (s->i(0, 1)) {
		math::vec3 temp = gradientTop;
		gradientTop = gradientBottom;
		gradientBottom = temp;
	}

	//
	// determine shift variation bounds
	//
	double sv = 1.0;

	//
	// determine fractal shift variation vector per iteration
	//
	shift.x = s->d(-sv, -sv / 32.0);
	shift.y = s->d(-sv, -sv / 32.0);
	shift.z = s->d(-sv, -sv / 32.0);
	std::cout << "shift x: " << shift.x << std::endl;
	std::cout << "shift y: " << shift.y << std::endl;
	std::cout << "shift z: " << shift.z << std::endl;

	double rv = 0.25;
	zr = s->d(-rv, -rv / 32.0);
	xr = s->d(-rv, -rv / 32.0);
	std::cout << "zr: " << zr << std::endl;
	std::cout << "xr: " << xr << std::endl;

	//
	// precompute sine and cosine of rotation angles
	//
	zrs = std::sin(zr);
	zrc = std::cos(zr);
	xrs = std::sin(xr);
	xrc = std::cos(xr);

	//
	// determine box distance estimator size
	//
	deSize = s->d(0.75, 1.25);

	//
	// determine shadow softness of the fractal
	//
	shadowSoftness = s->d(0.75, 1.0);
}

fractal::~fractal() {
}

void fractal::iteratePoint(math::vec3& point) {
	//
	// get absolute point position
	//
	point = math::absolute(point);
	//
	// apply point folds and rotations
	//
	math::rotation::x(point, xrs, xrc);
	math::rotation::z(point, zrs, zrc);
	math::fold::menger(point);
	math::rotation::z(point, zrs, zrc);
	math::rotation::x(point, xrs, xrc);
	math::fold::sierpinski(point);
	//
	// apply fractal shift
	//
	point += shift;
}

//
// main fractal distance estimator
//
double fractal::de(math::vec3 point) {
	for (int i = 0; i < iterations; ++i) {
		iteratePoint(point);
	}
	return math::de::box(point, deSize);
}

//
// shadowing technique. 
// explained in detal at inigo quilez's blog:
// https://iquilezles.org/www/articles/rmshadows/rmshadows.htm
//
double fractal::calculateShadow(math::ray r) {
	double res = 1.0;
	double ph = 1e20;
	double tmax = 10.0 * deSize;
	double t = 0.0001;
	//
	// kinda like raymarching the shadow with some fancy modifiers
	// to make it soft and round
	//
	for(; t < tmax; ) {
		double h = de(r.origin + r.direction * t);
		if (h < 0.0001) {
			return 0.0;
		}
		double y = h * h / (2.0 * ph);
		double d = std::sqrt(h * h - y * y);
		res = std::min(res, shadowSoftness * d / std::max(0.0, t - y));
		ph = h;
		t += h;
	}

	return res;
}

//
// orbit trap coloring method
//
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

//
// tetrahedron technique used to calculate normals instead of
// the classical forward and central differences technique.
// the main difference is the amount of calls to the DE function
// is being reduced from 6 to only 4. and that's a lot.
// explained in detal at inigo quilez blog:
// https://www.iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
//
math::vec3 fractal::calculateNormal(math::vec3 point) {
	double e = 0.0001;
	math::vec3 xyy(1.0, -1.0, -1.0);
	math::vec3 yyx(-1.0, -1.0, 1.0);
	math::vec3 yxy(-1.0, 1.0, -1.0);
	math::vec3 xxx(1.0, 1.0, 1.0);
	return math::normalize(
				xyy * de(point + xyy * e) +
				yyx * de(point + yyx * e) +
				yxy * de(point + yxy * e) +
				xxx * de(point + xxx * e)
				);
}
