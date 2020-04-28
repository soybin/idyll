#include "math.h"
#include <algorithm>

#define PI 3.14159265358979

namespace math {
	vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3::vec3(float v) : x(v), y(v), z(v) {}
	vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	
	float length(const vec3& r) {
		return std::sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	}
	vec3 normalize(vec3 v) {
		return v / length(v);
	}
	float mod(float x, float y) {
		return x - y * std::floor(x / y);
	}
	vec3 abs(vec3 v) {
		return vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}
	float max(const vec3& r) {
		return std::max(r.x, std::max(r.y, r.z));
	}
	vec3 max(const vec3& r1, const vec3& r2) {
		return vec3(std::max(r1.x, r2.x), std::max(r1.y, r2.y), std::max(r1.z, r2.z));
	}
	float dot(const vec3& r1, const vec3& r2) {
		return r1.x * r2.x, r1.y * r2.y, r1.z * r2.z;
	}

	vec3 calcRayDir(vec3 rot, float xcoord, float ycoord, int WIDTH, int HEIGHT, int FOV) {
		// find direction vector
		float x = xcoord - WIDTH / 2.0f;
		float y = ycoord - HEIGHT / 2.0f;
		float z = HEIGHT / std::tan(FOV * PI / 180.0f / 2.0f);
		vec3 a = normalize(vec3(x, y, -z));
		// compute rotation
			return a;
	}

	namespace fold {
		void menger(vec3& r) {
			float a = std::min(r.x - r.y, 0.0f);
			r.x -= a;
			r.y += a;
			a = std::min(r.x - r.z, 0.0f);
			r.x -= a;
			r.z += a;
			a = std::min(r.y - r.z, 0.0f);
			r.y -= a;
			r.z += a;
		}
	}

	namespace rotation {
		void x(vec3& r, float a) {
			float s = std::sin(a);
			float c = std::cos(a);
			r.y = r.y * c + r.z * s;
			r.z = r.z * c - r.y * s;
		}
		void y(vec3& r, float a) {
			float s = std::sin(a);
			float c = std::cos(a);
			r.x = r.x * c - r.z * s;
			r.z = r.z * c + r.x * s;
		}
		void z(vec3& r, float a) {
			float s = std::sin(a);
			float c = std::cos(a);
			r.x = r.x * c + r.y * s;
			r.y = r.y * c - r.x * s;
		}
	}

	namespace de {
		float sphere(vec3 point, float size) {
			//point.x = mod(point.x, 1.0f) - 0.5f;
			//point.y = mod(point.y, 1.0f) - 0.5f;
			//point.z = mod(point.z, 1.0f) - 0.5f;
			return length(point) - size;
		}
		float box(vec3 p, vec3 b) {
			vec3 a = abs(p) - b;
			return length(max(a, vec3(0.0f))) + std::min(max(a), 0.0f);
		}	
		float main(vec3 p, const vec3& rot, const vec3& shift, float scale, int iter) {
			for (int i = 0; i < iter; ++i) {
				p = abs(p);
				rotation::z(p, rot.z);
				fold::menger(p);
				rotation::x(p, rot.x);
				p *= scale;
				p += shift;
			}
			return box(p, vec3(6.0f));
		}
	}
}
