#include "math.h"
#include <algorithm>

#define PI 3.14159265358979

namespace math {

	// vector constructors
	vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	vec3::vec3(float v) : x(v), y(v), z(v) {}
	vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	//vector operator overloading
	vec3 vec3::operator + (const vec3& r) {
		return vec3(x + r.x, y + r.y, z + r.z);
	}
	vec3 vec3::operator - (const vec3& r) {
		return vec3(x - r.x, y - r.y, z - r.z);
	}
	vec3 vec3::operator * (const vec3& r) {
		return vec3(x * r.x, y * r.y, z * r.z);
	}
	vec3 vec3::operator * (float v) {
		return vec3(x * v, y * v, z * v);
	}
	vec3 vec3::operator / (const vec3& r) {
		return vec3(x / r.x, y / r.y, z / r.z);
	}
	vec3 vec3::operator / (float v) {
		return vec3(x / v, y / v, z / v);
	}
	void vec3::operator = (const vec3& r) {
		x = r.x;
		y = r.y;
		z = r.z;
	}
	bool vec3::operator == (const vec3& r) {
		return (x == r.x) && (y == r.y) && (z == r.z);
	}
	bool vec3::operator != (const vec3& r) {
		return (x != r.x) || (y != r.y) || (z != r.z);
	}
	void vec3::operator += (const vec3& r) {
		x += r.x;
		y += r.y;
		z += r.z;
	}
	void vec3::operator -= (const vec3& r) {
		x -= r.x;
		y -= r.y;
		z -= r.z;
	}
	void vec3::operator *= (const vec3& r) {
		x *= r.x;
		y *= r.y;
		z *= r.z;
	}
	void vec3::operator *= (float v) {
		x *= v;
		y *= v;
		z *= v;
	}
	void vec3::operator /= (const vec3& r) {
		x /= r.x;
		y /= r.y;
		z /= r.z;
	}
	void vec3::operator /= (float v) {
		x /= v;
		y /= v;
		z /= v;
	}

	//vector arithmetic functions
	float length(vec3 v) {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	vec3 normalize(vec3 v) {
		return v / length(v);
	}
	vec3 absolute(vec3 v) {
		return vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
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
			vec3 a = absolute(p) - b;	
			return length(vec3(std::max(a.x, 0.0f), std::max(a.y, 0.0f), std::max(a.z, 0.0f))) + std::min(std::max(a.x, std::max(a.y, a.z)), 0.0f);
		}
	}
}
