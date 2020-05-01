#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal() {
	fov = seed::i(60, 60);
	iter = seed::i(14, 16);
	cam = math::vec3(0.0f, 0.0f, 30.0f);
	col = math::vec3();
	rot.x = seed::f(-0.1f, 0.0f);
	rot.y = seed::f(-0.1f, 0.0f);
	rot.z = seed::f(-0.1f, 0.0f);
	shift.x = seed::f(-0.2f, 0.0f);
	shift.y = seed::f(-0.2f, 0.0f);
	shift.z = seed::f(-0.2f, 0.0f);
}

fractal::~fractal() {
}
