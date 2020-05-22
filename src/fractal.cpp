#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal(seed* s) : s(s) {

	//
	// get number of fractal iterations
	//
	iterations = s->values["iterations"];

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
	shift.x = s->values["xshift"];
	shift.y = s->values["yshift"];
	shift.z = s->values["zshift"];
	
	//
	// determine fractal rotation per iteration
	//
	xr = s->values["xrotation"];
	zr = s->values["zrotation"];

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
	deScale = s->values["deScale"];

	//
	// determine shadow softness of the fractal
	//
	shadowSoftness = s->values["shadowSoftness"];
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
	return math::de::box(point, deScale);
}

double fractal::calculateShadow(math::ray r) {
	double res = 1.0;
	double ph = 1e20;
	double tmax = 12.0;
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
	math::vec3 orbit(0.0);
	for (int i = 0; i < iterations; ++i) {
		iteratePoint(point);
		math::vec3 pc = point * color;
		orbit = math::vec3(std::max(pc.x, orbit.x), std::max(pc.y, orbit.y), std::max(pc.z, orbit.z));
	}
	return orbit;
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
