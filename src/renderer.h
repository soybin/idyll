/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#pragma once

#include "math.h"
#include "fractal.h"

#include <vector>

class renderer {
	private:
		int WIDTH;
		int HEIGHT;
		int FOV;
		int SAMPLES;
		int BOUNCES;
		double SKY_NOISE;
		double GLOSSINESS_CHANCE;
		double GLOSSINESS_AMOUNT;
		math::vec3 cameraPosition;
		math::vec3 lightDirection;
		math::vec3 lightColor;
		math::vec3 skyColor;
		std::vector<math::vec3> RMx;
		std::vector<math::vec3> RMy;

		// object pointers
		fractal* f;
		seed* s;

		// update rotation matrices so that the ray distance
		// calculation function works properly
		void updateRotationMatrix();

		// get ray direction based on screen coordinates and apply
		// yaw and pitch rotation to it
		math::vec3 calculateRayDirection(double y, double x);

		// ray march a ray. return negative if nothing was hit
		double march(math::ray r);

		// in case the ray dosn't hit a system
		math::vec3 renderSky(double y, double x);

		// bidirectional reflectance distribution function.
		// returns a random direction in which a normal might reflect
		// an incoming ray.
		// it might return either a direction generated by a cosine
		// distribution function (for lambertian reflection) or a value
		// generated by a cone distribution function (for glossy
		// reflection).
		math::vec3 brdf(math::vec3 direction, math::vec3 normal);

		// main rendering function
		math::vec3 pathTrace(math::ray r, int levelsLeft);

	public:
		renderer(int width, int height, seed* s, fractal* f);
		~renderer();

		math::vec3 render(double y, double x);
};
