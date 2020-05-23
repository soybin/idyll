#pragma once

#include "math.h"
#include "seed.h"
#include <vector>
#include <string>

class fractal {
	private:
		// fractal's variables. randomly set at runtime
		int iterations;
		int colorRange;
		double zr;
		double xr;
		double zrs;
		double zrc;
		double xrs;
		double xrc;
		double shadowSoftness;
		math::vec3 color;
		math::vec3 shift;

		// seed pointer
		seed* s;

		//
		// used when computing DE and color. 
		// builds the fractal.
		//
		void iteratePoint(math::vec3& point);
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
