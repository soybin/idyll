#include "config.h"
#include "fractal.h"
#include "gui.h"
#include "math.h"
#include "region.h"
#include <fstream>
#include <iostream>
#include <thread>

void renderThread(int rowStart, int rowEnd, int colStart, int colEnd, int chunk, fractal* f, std::vector<std::vector<math::vec3>>& image) {
	int w = 1920, h = 1080;
	region reg(w, h, f);
	for (int count = 0, ycoord = rowStart; ycoord < rowEnd || ycoord < h; ++ycoord) {
		reg.setYCoord((float)ycoord + 0.5f);
		for (int xcoord = colStart; xcoord < colEnd || (xcoord < w && count != chunk); ++xcoord) {
			reg.setXCoord((float)xcoord + 0.5f);
			image[ycoord][xcoord] = reg.render();
			++count;
		}
	}
}

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

	// coordinates
	float y = 0.5f, x = 0.5f;

	// gui
	gui::setup();
	//std::thread update(gui::update, &y, (float)height + 0.5f);

	// frac
	fractal* f = new fractal();
	while (math::de::main(f->cam, f->rot, f->shift, 1.0f, f->iter) < 1.0f) {
		f->cam += math::vec3(0.0f, 0.0f, 10.0f);
	}

	// multithread	
	std::vector<std::vector<math::vec3>> image(height, std::vector<math::vec3>(width, math::vec3()));
	int thread_cnt = 100;
	unsigned long long int fullsize = width * height;
	std::vector<std::thread> threads;
	int chunk = fullsize / thread_cnt;

	int lastI = 0, lastJ = 0;
	for (int threadNum = 0; threadNum < thread_cnt; ++threadNum) {
		int i = lastI;
		int j = lastJ;
		int cursize = 0;
		for (; i < height; ++i) {
			for (int j = 0; j < width && cursize != chunk; ++j) {
				++cursize;
			}
		}
		threads.push_back(renderThread, lastI, i, j, lastJ, chunk, f, image);
		lastI = i;
		lastJ = j;
	}
	
	for (auto& thread : threads) {
		thread.join();
	}

	if (fullsize % thread_cnt != 0) {
		renderThread(lastI, height - 1, lastJ, width - 1, chunk, f, image);
	}

	// old non-multithreaded rendering. very slow.
	/*
	for (; y < (float)height; ++y) {
		reg.setYCoord((float)height - y);
		for (float x = 0.5f; x < (float)width; ++x) {
			reg.setXCoord(x);
			math::vec3 pixel = reg.render();
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}
	*/

	// write to output file
	for (const auto& row : image) {
		for (const auto& pixel : row) {
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}

	// close gui thread
	// update.join();

	// free fractal <3
	delete f;

	return 0;
}
