/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

#pragma once

#include <cmath>

namespace math {
	struct vec3 {
		double x, y, z;
		vec3();
		vec3(double v);
		vec3(double x, double y, double z);

		// operator overloading
		
		// basic arithmetic
		vec3 operator + (const vec3& r);
		vec3 operator - (const vec3& r);
		vec3 operator * (const vec3& r);
		vec3 operator * (double v);
		vec3 operator / (const vec3& r);
		vec3 operator / (double v);

		// assignment operator
		void operator = (const vec3& r);

		// comparison operators
		bool operator == (const vec3& r);
		bool operator != (const vec3& r);

		// arithmetic and assignment operators
		void operator += (const vec3& r);
		void operator -= (const vec3& r);
		void operator *= (const vec3& r);
		void operator *= (double v);
		void operator /= (const vec3& r);
		void operator /= (double v);
	};

	struct ray {
		ray(vec3 o, vec3 d) : origin(o), direction(d) {}
		vec3 origin;
		vec3 direction;
	};

	//
	// vector arithmetic functions, based off GLSL4 reference
	// by khronos:
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/
	//
	extern double length(vec3 v);
	extern double dot(vec3 v1, vec3 v2);
	extern double clamp(double v, double rangeMin, double rangeMax);
	extern double smoothstep(double e0, double e1, double x);
	extern vec3 normalize(vec3 v);
	extern vec3 absolute(vec3 v);
	extern vec3 cross(vec3 v1, vec3 v2);
	extern vec3 mix(vec3 rangeMin, vec3 rangeMax, double v);
	extern vec3 clamp(vec3 v, double rangeMin, double rangeMax);
	extern vec3 reflect(vec3 i, vec3 n);

	// folds
	namespace fold {
		extern void plane(vec3& r, vec3 n, double d);
		extern void sierpinski(vec3& r);
		extern void menger(vec3& r);
	}

	// rotations
	namespace rotation {
		extern void x(vec3& r, double s, double c);
		extern void y(vec3& r, double s, double c);
		extern void z(vec3& r, double s, double c);
	}

	// distance estimators
	namespace de {
		extern double sphere(vec3 p, double size);
		extern double box(vec3 p, double size);
	}
}
