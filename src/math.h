#pragma once

#include <cmath>

namespace math {
	struct vec3 {
		float x, y, z;
		vec3();
		vec3(float v);
		vec3(float x, float y, float z);
		vec3 operator + (const vec3& r) const {
			return vec3(x + r.x, y + r.y, z + r.z);
		}
		vec3 operator - (const vec3& r) const {
			return vec3(x - r.x, y - r.y, z - r.z);
		}
		vec3 operator * (const vec3& r) const {
			return vec3(x * r.x, y * r.y, z * r.z);
		}
		vec3 operator * (float v) const {
			return vec3(x * v, y * v, z * v);
		}
		vec3 operator / (const vec3& r) const {
			return vec3(x / r.x, y / r.y, z / r.z);
		}
		vec3 operator / (float v) const {
			return vec3(x / v, y / v, z / v);
		}	
	};

	// simple mathematical functions based off
	// glsl implementation. docs.gl
	extern float length(const vec3& r);
	extern vec3 normalize(vec3 r);
	extern float mod(float x, float y);

	// region specific functions
	extern vec3 calcRayDir(float xcoord, float ycoord, int WIDTH, int HEIGHT, int FOV);

	// distance estimators
	namespace de {
		extern float sphere(vec3 p);
	}
}
