#pragma once

#include <cmath>

namespace math {
	struct vec3 {
		float x, y, z;
		vec3();
		vec3(float v);
		vec3(float x, float y, float z);

		// operator overloading
		
		// basic arithmetic
		vec3 operator + (const vec3& r);
		vec3 operator - (const vec3& r);
		vec3 operator * (const vec3& r);
		vec3 operator * (float v);
		vec3 operator / (const vec3& r);
		vec3 operator / (float v);

		// assignment operator
		void operator = (const vec3& r);

		// comparison operators
		bool operator == (const vec3& r);
		bool operator != (const vec3& r);

		// arithmetic and assignment operators
		void operator += (const vec3& r);
		void operator -= (const vec3& r);
		void operator *= (const vec3& r);
		void operator *= (float v);
		void operator /= (const vec3& r);
		void operator /= (float v);
	};

	extern float length(vec3 v);
	extern vec3 normalize(vec3 v);
	extern vec3 absolute(vec3 v);

	// region specific functions
	extern vec3 calcRayDir(vec3 rot, float xcoord, float ycoord, int WIDTH, int HEIGHT, int FOV);

	// folds
	namespace fold {
		extern void plane(vec3& r, vec3 n, float d);
		extern void sierpinski(vec3& r);
		extern void menger(vec3& r);
	}

	// rotations
	namespace rotation {
		extern void x(vec3& r, float a);
		extern void y(vec3& r, float a);
		extern void z(vec3& r, float a);
	}

	// distance estimators
	namespace de {
		extern float sphere(vec3 p, float size);
		extern float box(vec3 p, vec3 s);
	}
}
