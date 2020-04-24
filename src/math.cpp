#include "math.h"

#define PI 3.14159265358979

namespace math {
	vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3::vec3(float v) : x(v), y(v), z(v) {}
	vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	
	float length(const vec3& r) {
		return std::sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	}
	vec3 normalize(vec3 r) {
		return r / length(r);
	}
	float mod(float x, float y) {
		return x - y * std::floor(x / y);
	}

	vec3 calcRayDir(float xcoord, float ycoord, int WIDTH, int HEIGHT, int FOV) {
		float x = xcoord - WIDTH / 2.0f;
		float y = ycoord - HEIGHT / 2.0f;
		float z = HEIGHT / std::tan(FOV * PI / 180.0f / 2.0f);
		return normalize(vec3(x, y, -z));
	}

	namespace de {
		float sphere(vec3 point) {
			point.x = mod(point.x, 1.0f) - 0.5f;
			point.y = mod(point.y, 1.0f) - 0.5f;
			point.z = mod(point.z, 1.0f) - 0.5f;
			return length(point) - 0.2f;
		}
	}
}
