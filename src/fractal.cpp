#include "fractal.h"
#include "seed.h"

#include <iostream>

fractal::fractal() {
	fov = seed::i(60, 60);
	iter = seed::i(14, 16);
	cam.x = 0.0f;
	cam.y = 0.0f;
	cam.z = 40.0f;
	col = math::vec3(0.0f);
	rot.x = -0.1f;//seed::f(-0.4f, 0.0f);
	rot.y = -0.1f;//seed::f(-0.4f, 0.0f);
	rot.z = -0.1f;//seed::f(-0.4f, 0.0f);
	shift.x = -0.1f;//seed::f(-0.5f, 0.0f);
	shift.y = -0.1f;//seed::f(-0.5f, 0.0f);
	shift.z = -0.1f;//seed::f(-0.5f, 0.0f);
}

fractal::~fractal() {
}
