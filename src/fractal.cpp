/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#include "fractal.h"
#include "seed.h"

#include <iostream>

//                                                //
//======== p o i n t    i t e r a t o r s ========//
//                                                //

struct PI0 : pointIterator {
	PI0(double xs, double zs, double xrs, double xrc, double zrs, double zrc) : pointIterator(xs, zs, xrs, xrc, zrs, zrc) {}
	inline void iterate(math::vec3& point) {
		point = math::absolute(point);
		math::rotation::x(point, xrs, xrc);
		math::rotation::z(point, zrs, zrc);
		math::fold::menger(point);
		math::rotation::z(point, zrs, zrc);
		math::rotation::x(point, xrs, xrc);
		math::fold::sierpinski(point);
		point.x += xs;
		point.z += zs;
	}
};

struct PI1 : pointIterator {
	PI1(double xs, double zs, double xrs, double xrc, double zrs, double zrc) : pointIterator(xs, zs, xrs, xrc, zrs, zrc) {}
	inline void iterate(math::vec3& point) {
		point = math::absolute(point);
		math::rotation::z(point, zrs, zrc);
		math::rotation::x(point, xrs, xrc);
		math::fold::sierpinski(point);
		math::rotation::x(point, xrs, xrc);
		math::rotation::z(point, zrs, zrc);
		math::fold::menger(point);
		point.x += xs;
		point.z += zs;
	}
};

struct PI2 : pointIterator {
	PI2(double xs, double zs, double xrs, double xrc, double zrs, double zrc) : pointIterator(xs, zs, xrs, xrc, zrs, zrc) {}
	inline void iterate(math::vec3& point) {
		point = math::absolute(point);
		math::rotation::z(point, zrs, zrc);
		math::rotation::x(point, xrs, xrc);
		math::fold::sierpinski(point);
		math::fold::menger(point);
		math::rotation::x(point, xrs, xrc);
		math::rotation::z(point, zrs, zrc);
		point.x += xs;
		point.z += zs;
	}
};

//                                            //
//======== f r a c t a l    c l a s s ========//
//                                            //

fractal::fractal(seed* s) : s(s) {

	//
	// get number of fractal iterations
	//
	iterations = s->values["iterations"];

	//
	// determine shadow softness of the fractal
	//
	shadowSoftness = s->values["shadowSoftness"];

	//
	// get fractal color
	//
	color.x = s->values["xcolor"];
	color.y = s->values["ycolor"];
	color.z = s->values["zcolor"];

	//
	// get top sky gradient color
	// 
	gradientTop.x = s->values["xgradientTop"];
	gradientTop.y = s->values["ygradientTop"];
	gradientTop.z = s->values["zgradientTop"];

	//
	// get bottom sky gradient color
	//
	gradientBottom.x = s->values["xgradientBottom"];
	gradientBottom.y = s->values["ygradientBottom"];
	gradientBottom.z = s->values["zgradientBottom"];

	//
	// randomly change gradient color positions
	//
	if (s->i(0, 1)) {
		math::vec3 temp = gradientTop;
		gradientTop = gradientBottom;
		gradientBottom = temp;
	}

	//
	// determine fractal shift variation vector per iteration
	//
	xs = s->values["xshift"];
	zs = s->values["zshift"];
	
	//
	// determine fractal rotation per iteration
	//
	xr = s->values["xrotation"];
	zr = s->values["zrotation"];

	//
	// precompute sine and cosine of rotation angles
	//
	xrs = std::sin(xr);
	xrc = std::cos(xr);
	zrs = std::sin(zr);
	zrc = std::cos(zr);
	
	//
	// initialize point iterator
	//
	switch ((int)s->values["pointIterator"]) {
		case 0:
			mainPI = new PI0(xs, zs, xrs, xrc, zrs, zrc);
			break;
		case 1:
			mainPI = new PI1(xs, zs, xrs, xrc, zrs, zrc);
			break;
		case 2:
			mainPI = new PI2(xs, zs, xrs, xrc, zrs, zrc);
			break;
	}
}

fractal::~fractal() {
	delete mainPI;
}

//
// main fractal distance estimator
//
double fractal::de(math::vec3 point) {
	for (int i = 0; i < iterations; ++i) {
		mainPI->iterate(point);
	}
	return math::de::box(point, 1.0);
}

double fractal::calculateShadow(math::ray r) {
	double res = 1.0;
	double ph = 1e20;
	double tmax = 16.0;
	double t = 0.0001;

	//
	// kinda like raymarching the shadow with some fancy modifiers
	// to make it soft and round
	//
	for(; t < tmax; ) {
		double h = de(r.origin + r.direction * t);
		if (h < 0.001) {
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

math::vec3 fractal::calculateColor(math::vec3 point) {
	mainPI->iterate(point);
	math::vec3 pc = point * color;
	return math::vec3(std::max(0.0, pc.x), std::max(0.0, pc.y), std::max(0.0, pc.z));
}

math::vec3 fractal::calculateNormal(math::vec3 point) {
	double e = 0.00001;
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
