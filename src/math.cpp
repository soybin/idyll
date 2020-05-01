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
	float vec3::length() {
		return std::sqrt(x * x + y * y + z * z);
	}
	vec3 vec3::normalize() {
		return *this / length();
	}
	vec3 vec3::absolute() {
		return vec3(std::abs(x), std::abs(y), std::abs(z));
	}

	vec3 calcRayDir(vec3 rot, float xcoord, float ycoord, int WIDTH, int HEIGHT, int FOV) {
		// find direction vector
		float x = xcoord - WIDTH / 2.0f;
		float y = ycoord - HEIGHT / 2.0f;
		float z = HEIGHT / std::tan(FOV * PI / 180.0f / 2.0f);
		vec3 a = vec3(x, y, -z).normalize();
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

	namespace color {
		vec3 main(vec3 p, vec3 col, const vec3& rot, const vec3& shift, float scale, int iter) {
			vec3 orbit(0.0f);
			for (int i = 0; i < iter; ++i) {
				p = p.absolute();
				rotation::z(p, rot.z);
				fold::menger(p);
				rotation::x(p, rot.x);
				p *= scale;
				p += shift;
				vec3 pc = p * col;
				orbit = vec3(std::max(orbit.x, pc.x), std::max(orbit.y, pc.y), std::max(orbit.z, pc.z));
			}
			return orbit;
		}

		vec3 smooth(vec3 p, vec3 s1, vec3 s2, float dx, vec3 col, const vec3& rot, const vec3& shift, float scale, int iter) {
			return (main(p+vec3(s1)*dx, col, rot, shift, scale, iter)+
							main(p-vec3(s1)*dx, col, rot, shift, scale, iter)+
							main(p+vec3(s2)*dx, col, rot, shift, scale, iter)+
							main(p-vec3(s2)*dx, col, rot, shift, scale, iter));
		}
	}

	namespace de {
		float sphere(vec3 point, float size) {
			//point.x = mod(point.x, 1.0f) - 0.5f;
			//point.y = mod(point.y, 1.0f) - 0.5f;
			//point.z = mod(point.z, 1.0f) - 0.5f;
			return point.length() - size;
		}

		float box(vec3 p, vec3 b) {
			vec3 a = p.absolute() - b;
			
			return vec3(std::max(a.x, 0.0f), std::max(a.y, 0.0f), std::max(a.z, 0.0f)).length() + std::min(std::max(a.x, std::max(a.y, a.z)), 0.0f);
		}


		float main(vec3 p, const vec3& rot, const vec3& shift, float scale, int iter) {
			for (int i = 0; i < iter; ++i) {
				p = p.absolute();
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
