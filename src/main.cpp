#include "config.h"
#include "fractal.h"
#include "gui.h"
#include "math.h"
#include "region.h"
#include <fstream>
#include <iostream>
#include <thread>

// this function wil be instantiated in multiple threads at runtime. it iterates
// through a specified range in a 2d matrix and renders every pixel in the range.
// then it stores the pixel value at the "image" 2d matrix
void renderRange(int startRow, int endRow, int startCol, int endCol, int width, int height, int chunk, region* reg, std::vector<std::vector<math::vec3>>* image) {
	for (int y = startRow, x = startCol; y < height && (y < endRow || chunk > 0); ++y) {
		for (; x < width && (x < endCol || chunk > 0); ++x) {
			(*image)[y][x] = reg->render((double)height - ((double)y + 0.5), (double)x + 0.5);
			--chunk;
		}
		x = 0;
	}
}

int main(int argc, char* argv[]) {
	// coordinates
	double y = 0.5, x = 0.5;

	// gui
	gui::setup();

	// get config info
	int width = config::getInt("width");
	int height = config::getInt("height");
	int threadCount = config::getInt("threads");

	// pointer to fractal class
	fractal* f = new fractal();

	// pointer to fractal region
	region* reg = new region(width, height, f);

	// pointer to 2d matrix of pixels. it'll be accessible
	// by every thread
	std::vector<std::vector<math::vec3>>* image = new std::vector<std::vector<math::vec3>>(height, std::vector<math::vec3>(width, math::vec3()));

	// thread array
	std::vector<std::thread> threads;

	// thread constants
	int chunk = width * height / threadCount;
	int lastY = 0;
	int lastX = 0;

	// thread assignment loop. this loop assigns to every thread
	// the render function with the right coordinates and stores
	// it on the thread array	
	for (int i = 0; i < threadCount; ++i) {
		int y = lastY;
		int x = lastX;
		int count = 0;
		for (; y < height && count < chunk; ) {
			for (; x < width && count < chunk; ) {
				if (++count == chunk) {
					break;
				}
				++x;
			}
			if (count < chunk) {
				x = 0;
				++y;
			}
		}

		// skip the first chunk because that one will be rendered
		// by the main thread later
		if (i > 0) {
			threads.push_back(std::thread{renderRange, lastY, y, lastX, x, width, height, chunk, reg, image});
		}

		// advance one coordinate to set the starter coordinate
		// for the next loop, since this coordinate has already
		// been enqueued to be computed in the previously called
		// thread
		if (x + 1 == width) {
			x = 0;
			++y;
		} else {
			++x;
		}

		// save starter coordinates
		lastY = y;
		lastX = x;
	}

	// fill the remaining image portion which is smaller
	// than one chunk in case there's any
	if ((width * height) % threadCount != 0) {
		threads.push_back(std::thread{renderRange, lastY, height, lastX, width, width, height, chunk, reg, image});
	}

	// render the first chunk in the main application thread
	// so that it can update the gui progress bar
	int count = 0;
	std::thread{gui::update, &count, chunk}.detach();
	for (int y = 0; y < height && count < chunk; ++y) {
		for (int x = 0; x < width && count < chunk; ++x) {
			(*image)[y][x] = reg->render((double)height - ((double)y + 0.5f), (double)x + 0.5);
			++count;
		}
	}

	// wait for rendering threads to finish
	for (auto& thread : threads) {
		thread.join();
	}

	// write to output file
	std::ofstream out("out.ppm");
	out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			math::vec3 pixel = (*image)[i][j];
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}

	// free memory
	delete f;
	delete reg;
	delete image;

	return 0;
}
