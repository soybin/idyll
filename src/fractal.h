/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#pragma once

#include "math.h"
#include "seed.h"
#include <vector>
#include <string>

struct pointIterator {
	double xs;
	double zs;
	double xrs;
	double xrc;
	double zrs;
	double zrc;
	pointIterator(double xs, double zs, double xrs, double xrc, double zrs, double zrc) : xs(xs), zs(zs), xrs(xrs), xrc(xrc), zrs(zrs), zrc(zrc) {}
	virtual void iterate(math::vec3& point) = 0;
};

class fractal {
	private:
		// fractal's variables. randomly set at runtime
		int iterations;
		double xs;
		double zs;
		double xr;
		double zr;
		double xrs;
		double xrc;
		double zrs;
		double zrc;
		double shadowSoftness;
		math::vec3 color;

		// seed pointer
		seed* s;

		// polymorphic point iterator
		pointIterator* mainPI;

	public:
		math::vec3 gradientTop;
		math::vec3 gradientBottom;

		fractal(seed* s);
		~fractal();

		// main distance estimator
		double de(math::vec3 point);

		//
		// smooth shadowing technique. 
		// explained in detal at inigo quilez's blog:
		// https://iquilezles.org/www/articles/rmshadows/rmshadows.htm
		//
		double calculateShadow(math::ray r);

		// fractal coloring using the orbit trap technique
		math::vec3 calculateColor(math::vec3 point);

		// tetrahedron technique used to calculate normals instead of
		// the classical forward and central differences technique.
		// the main difference is the amount of calls to the DE function
		// is being reduced from 6 to only 4. and that's a lot.
		// explained in detal at inigo quilez blog:
		// https://www.iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
		math::vec3 calculateNormal(math::vec3 point);
};
