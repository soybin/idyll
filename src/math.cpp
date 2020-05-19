#include "math.h"
#include <algorithm>

#define PI 3.14159265358979

namespace math {

	// vector constructors
	vec3::vec3() : x(0.0), y(0.0), z(0.0) {}
	vec3::vec3(double v) : x(v), y(v), z(v) {}
	vec3::vec3(double x, double y, double z) : x(x), y(y), z(z) {}

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
	vec3 vec3::operator * (double v) {
		return vec3(x * v, y * v, z * v);
	}
	vec3 vec3::operator / (const vec3& r) {
		return vec3(x / r.x, y / r.y, z / r.z);
	}
	vec3 vec3::operator / (double v) {
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
	void vec3::operator *= (double v) {
		x *= v;
		y *= v;
		z *= v;
	}
	void vec3::operator /= (const vec3& r) {
		x /= r.x;
		y /= r.y;
		z /= r.z;
	}
	void vec3::operator /= (double v) {
		x /= v;
		y /= v;
		z /= v;
	}

	//vector arithmetic functions
	double length(vec3 v) {
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	double dot(vec3 v1, vec3 v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
	double clamp(double v, double rangeMin, double rangeMax) {
		return std::min(std::max(v, rangeMin), rangeMax);
	}
	double smoothstep(double e0, double e1, double x) {
    double t = clamp((x - e0) / (e1 - e0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
	}
	vec3 normalize(vec3 v) {
		return v / length(v);
	}
	vec3 absolute(vec3 v) {
		return vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}
	vec3 cross(vec3 v1, vec3 v2) {
		return vec3(v1.y * v2.z - v2.y * v1.z,
								v1.z * v2.x - v2.z * v1.x,
								v1.x * v2.y - v2.x * v1.y);
	}
	vec3 mix(vec3 rangeMin, vec3 rangeMax, double v) {
		return rangeMin * (1.0 - v) + rangeMax * v;
	}
	vec3 clamp(vec3 v, double rangeMin, double rangeMax) {
		v.x = std::min(std::max(v.x, rangeMin), rangeMax);
		v.y = std::min(std::max(v.y, rangeMin), rangeMax);
		v.z = std::min(std::max(v.z, rangeMin), rangeMax);
		return v;
	}

	namespace fold {
		void sierpinski(vec3& r) {
			double a = std::min(r.x + r.y, 0.0);
			double b = std::min(r.x + r.z, 0.0);
			double c = std::min(r.z + r.y, 0.0);
			r.x -= a;
			r.y -= a;
			r.x -= b;
			r.z -= b;
			r.y -= c;
			r.z -= c;
		}
		void menger(vec3& r) {
			double a = std::min(r.x - r.y, 0.0);
			r.x -= a;
			r.y += a;
			a = std::min(r.x - r.z, 0.0);
			r.x -= a;
			r.z += a;
			a = std::min(r.y - r.z, 0.0);
			r.y -= a;
			r.z += a;
		}
	}

	namespace rotation {
		void x(vec3& r, double s, double c) {
			r.y = r.y * c + r.z * s;
			r.z = r.z * c - r.y * s;
		}
		void y(vec3& r, double s, double c) {
			r.x = r.x * c - r.z * s;
			r.z = r.z * c + r.x * s;
		}
		void z(vec3& r, double s, double c) {
			r.x = r.x * c + r.y * s;
			r.y = r.y * c - r.x * s;
		}
	}

	namespace de {
		double sphere(vec3 point, double size) {
			return length(point) - size;
		}

		double box(vec3 p, double size) {
			vec3 a = absolute(p) - 1.0;	
			return length(vec3(std::max(a.x, 0.0), std::max(a.y, 0.0), std::max(a.z, 0.0))) + std::min(std::max(a.x, std::max(a.y, a.z)), 0.0);
		}
	}
}
