#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal() {
	iterations = seed::i(16, 24);
	scale = seed::f(4.0, 6.0);

	int colorFirst = seed::i(1, 3);
	switch (colorFirst) {
		case 1:
			color.x = seed::f(0.5, 1.0);
			color.z = seed::f(0.0, 0.5);
			color.y = seed::f(0.0, 0.5);
			break;
		case 2:
			color.z = seed::f(0.5, 1.0);
			color.x = seed::f(0.0, 0.5);
			color.y = seed::f(0.0, 0.5);
			break;
		case 3:
			color.y = seed::f(0.5, 1.0);
			color.x = seed::f(0.0, 0.5);
			color.z = seed::f(0.0, 0.5);
			break;
	}
	
	double rangeR = 0.1;
	double rangeS = 0.1;
	operations = {
		{'R', 'X', seed::f(-rangeR, 0.0)},
		{'R', 'Z', seed::f(-rangeR, 0.0)},
		{'R', 'Y', seed::f(-rangeR, 0.0)},
		{'S', 'X', seed::f(-rangeS, rangeS)},
		{'S', 'Y', seed::f(-rangeS, rangeS)},
		{'S', 'Z', seed::f(-rangeS, rangeS)},
		{'F', '0', 0.0},
		{'R', 'X', seed::f(-rangeR, 0.0)},
		{'R', 'Y', seed::f(-rangeR, 0.0)},
		{'R', 'Z', seed::f(-rangeR, 0.0)},
		{'S', 'X', seed::f(-rangeS, rangeS)},
		{'S', 'Y', seed::f(-rangeS, rangeS)},
		{'S', 'Z', seed::f(-rangeS, rangeS)},
		{'F', '0', 0.0}
	};
}

fractal::~fractal() {
}

void fractal::iteratePoint(math::vec3& point) {
	point = math::absolute(point);
	for (operation op : operations) {
		switch(op.action) {
			// rotation action
			case 'R':
				switch (op.specification) {
					case 'X':
						math::rotation::x(point, op.value);
						break;
					case 'Y':
						math::rotation::y(point, op.value);
						break;
					case 'Z':
						math::rotation::z(point, op.value);
						break;
				}
				break;

			// shift action
			case 'S':
				switch (op.specification) {
					case 'X':
						point.x += op.value;
						break;
					case 'Y':
						point.y += op.value;
						break;
					case 'Z':
						point.z += op.value;
				}
				break;

			// fold action
			case 'F':
				math::fold::menger(point);
				break;
		}
	}
}

double fractal::de(math::vec3 point) {
	for (int i = 0; i < iterations; ++i) {
		iteratePoint(point);
	}
	return math::de::box(point, math::vec3(scale));
}

// soft shadowing technique. explained in detal at inigo quilez's blog
// https://iquilezles.org/www/articles/rmshadows/rmshadows.htm
double fractal::calculateShadow(math::vec3 point, math::vec3 lightDirection) {
	// warning! hardcoded values!
	//
	// roughness of the shadow. the lower it is, the harder the 
	// shadow looks
	double radius = 0.5;
	// min and max distance to find/intersect shadow; the greater
	// this values, the darker it'll be
	double mint = 0.01;
	double maxt = 0.05;

	double res = 1.0;
	double ph = 1e20;
	for (double t = mint; t < maxt;) {
		double h = de(point + lightDirection * t);
		if (h < 0.001) {
			return 0.0;
		}
		double y = h * h / (2.0 * ph);
		double d = std::sqrt(h * h - y * y);
		res = std::min(res, radius * d / std::max(0.0, t - y));
		ph = h;
		t += h;
	}
	return res;
}

math::vec3 fractal::calculateColor(math::vec3 point, math::vec3 s1, math::vec3 s2, double radius) {
	std::vector<math::vec3> points(4);
	points[0] = point + (s1 * radius);
	points[1] = point - (s1 * radius);
	points[2] = point + (s2 * radius);
	points[3] = point - (s2 * radius);
	math::vec3 colorSum(0.0);
	for (math::vec3& curPoint : points) {
		// basic coloring function for each of the points in the
		// radius. it basically does the same as the de function
		// but computing the trap orbit as well
		math::vec3 orbit(0.0);
		for (int i = 0; i < iterations; ++i) {
			// do the fractal transformation of the point
			iteratePoint(curPoint);
			// orbit trap coloring
			math::vec3 pc = curPoint * color;
			orbit = math::vec3(std::max(orbit.x, pc.x), std::max(orbit.y, pc.y), std::max(orbit.z, pc.z));
		}
		colorSum += orbit;
	}

	// get the average color within the colors in the radius from the
	// original point
	return (colorSum / 4.0);
}

// tetrahedron technique. this is the most efficient way to calculate
// a surface normal at a specific point since it only takes four
// calls to the distance estimator insted of the six calls required
// by the classical forward and central differences technique.
// explained in detal at inigo quilez blog
// https://www.iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
math::vec3 fractal::calculateNormal(math::vec3 point, double radius) {
	// the four tetrahedron vertices
	math::vec3 v1(1.0, -1.0, -1.0);
	math::vec3 v2(-1.0, -1.0, 1.0);
	math::vec3 v3(-1.0, 1.0, -1.0);
	math::vec3 v4(1.0, 1.0, 1.0);
	return normalize(
			v1 * de(point + v1 * radius) +
			v2 * de(point + v2 * radius) +
			v3 * de(point + v3 * radius) +
			v4 * de(point + v4 * radius)
	);
}
