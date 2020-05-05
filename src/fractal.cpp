#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal() {
	iterations = 20;//seed::i(16, 24);
	scale = seed::f(6.0f, 6.0f);
	color = math::vec3(0.53f, 0.1f, 0.94f);
	operations = {{'R', 'X', -0.2f}, {'S', 'Y', 0.12f}, {'F', '0', 0.0f} ,{'R', 'Z', -0.1f}};
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

float fractal::de(math::vec3 point) {
	for (int i = 0; i < iterations; ++i) {
		iteratePoint(point);
	}
	return math::de::box(point, math::vec3(scale));
}

// soft shadowing technique. explained in detal at inigo quilez's blog
// https://iquilezles.org/www/articles/rmshadows/rmshadows.htm
float fractal::calculateShadow(math::vec3 point, math::vec3 lightDirection) {
	// warning! hardcoded values!
	//
	// roughness of the shadow. the lower it is, the harder the 
	// shadow looks
	float radius = 0.7f;
	// min and max distance to find/intersect shadow; the greater
	// this values, the darker it'll be
	float mint = 0.01f;
	float maxt = 0.05f;

	float res = 1.0f;
	float ph = 1e20;
	for (float t = mint; t < maxt;) {
		float h = de(point + lightDirection * t);
		if (h < 0.001f) {
			return 0.0f;
		}
		float y = h * h / (2.0f * ph);
		float d = std::sqrt(h * h - y * y);
		res = std::min(res, radius * d / std::max(0.0f, t - y));
		ph = h;
		t += h;
	}
	return res;

	// this version works for most of the cases but produces weird
	// lighting artifacts in some cases whenever the geometry gets 
	// more complex.
	//
	/*float radius = 0.25f;
	float s = 1.0f;
	int step = 0;
	for (float t = 0.1f; t < 0.5f && step < 500; ++step) {
		float h = de(point + lightDirection * t);
		s = std::min(s, 0.5f + 0.5f * h/(radius * t));
		if(s < 0.0f) break;
		t += h;
	}
	s = std::max(s, 0.0f);
	// smoothinggg
	return s * s * (3.0f - 2.0f * s);*/
}

math::vec3 fractal::calculateColor(math::vec3 point, math::vec3 s1, math::vec3 s2, float radius) {
	std::vector<math::vec3> points(4);
	points[0] = point + (s1 * radius);
	points[1] = point - (s1 * radius);
	points[2] = point + (s2 * radius);
	points[3] = point - (s2 * radius);
	math::vec3 colorSum(0.0f);
	for (math::vec3& curPoint : points) {
		// basic coloring function for each of the points in the
		// radius. it basically does the same as the de function
		// but computing the trap orbit as well
		math::vec3 orbit(0.0f);
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
	return (colorSum / 4.0f / iterations);
}

// tetrahedron technique. this is the most efficient way to calculate
// a surface normal at a specific point since it only takes four
// calls to the distance estimator insted of the six calls required
// by the classical forward and central differences technique.
// explained in detal at inigo quilez blog
// https://www.iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
math::vec3 fractal::calculateNormal(math::vec3 point, float radius) {
	// the four tetrahedron vertices
	math::vec3 v1(1.0f, -1.0f, -1.0f);
	math::vec3 v2(-1.0f, -1.0f, 1.0f);
	math::vec3 v3(-1.0f, 1.0f, -1.0f);
	math::vec3 v4(1.0f, 1.0f, 1.0f);
	return normalize(
			v1 * de(point + v1 * radius) +
			v2 * de(point + v2 * radius) +
			v3 * de(point + v3 * radius) +
			v4 * de(point + v4 * radius)
	);
}
