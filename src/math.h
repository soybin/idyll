#pragma once

#include <cmath>

namespace math {
	struct vec3 {
		float x, y, z;
		vec3();
		vec3(float v);
		vec3(float x, float y, float z);
		inline vec3 operator + (const vec3& r) const {
			return vec3(x + r.x, y + r.y, z + r.z);
		}
		inline vec3 operator - (const vec3& r) const {
			return vec3(x - r.x, y - r.y, z - r.z);
		}
		inline vec3 operator * (const vec3& r) const {
			return vec3(x * r.x, y * r.y, z * r.z);
		}
		inline vec3 operator * (float v) const {
			return vec3(x * v, y * v, z * v);
		}
		inline vec3 operator / (const vec3& r) const {
			return vec3(x / r.x, y / r.y, z / r.z);
		}
		inline vec3 operator / (float v) const {
			return vec3(x / v, y / v, z / v);
		}
		inline vec3& operator += (const vec3& r) {
			x += r.x;
			y += r.y;
			x += r.z;
			return *this;
		}
		inline vec3& operator -= (const vec3& r) {
			x -= r.x;
			y -= r.y;
			z -= r.z;
			return *this;
		}
		inline vec3& operator *= (const vec3& r) {
			x *= r.x;
			y *= r.y;
			z *= r.z;
			return *this;
		}
		inline vec3& operator *= (float v) {
			x *= v;
			y *= v;
			z *= v;
			return *this;
		}
		inline vec3& operator /= (const vec3& r) {
			x /= r.x;
			y /= r.y;
			z /= r.z;
			return *this;
		}
		inline vec3& operator /= (float v) {
			x /= v;
			y /= v;
			z /= v;
			return *this;
		}
	};

	// simple mathematical functions based off
	// glsl implementation. docs.gl
	extern float length(const vec3& r);
	extern vec3 normalize(vec3 r);
	extern float mod(float x, float y);
	extern vec3 abs(vec3 v);
	extern float max(const vec3& r);
	extern vec3 max(const vec3& r1, const vec3& r2);
	extern float dot(const vec3& r1, const vec3& r2);

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
		extern float main(vec3 p, const vec3& rot, const vec3& shift, float scale, int iter);
	}
}
