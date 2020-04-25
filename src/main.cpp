#include "config.h"
#include "gui.h"
#include "math.h"
#include "seed.h"
#include "region.h"
#include <fstream>
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
	// get config info
	int width = config::getInt("width");
	int height = config::getInt("height");
	if (width < 1 || height < 1) {
		return 0;
	}

	// initialize output file
	std::ofstream out("out.ppm");
	out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';

	// create seeded region
	region* reg = new region(width, height);
	
	// coordinates
	float y = 0.5f, x = 0.5f;

	// gui
	gui::setup();
	std::thread update(gui::update, &y, (float)height + 0.5f);

	for (; y < (float)height; ++y) {
		reg->setYCoord((float)height - y);
		for (float x = 0.5f; x < (float)width; ++x) {
			reg->setXCoord(x);
			math::vec3 pixel = reg->render();
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}

	// close gui thread
	update.join();

	// free fractal <3
	delete reg;

	return 0;
}
