#include "config.h"
#include "fractal.h"
#include "gui.h"
#include "math.h"
#include "seed.h"
#include "renderer.h"
#include <fstream>
#include <iostream>
#include <thread>

// png lib by Nayuki (https://www.nayuki.io/page/tiny-png-output)
#include "TinyPngOut.hpp"

// this function wil be instantiated in multiple threads at runtime. it iterates
// through a specified range in a 2d matrix and renders every pixel in the range.
// then it stores the pixel value at the "image" 2d matrix
void renderRange(int startRow, int endRow, int startCol, int endCol, int width, int height, int chunk, renderer* r, std::vector<std::vector<math::vec3>>* image) {
	for (int y = startRow, x = startCol; y < height && (y < endRow || chunk > 0); ++y) {
		for (; x < width && (x < endCol || chunk > 0); ++x) {
			(*image)[y][x] = r->render((double)height - ((double)y + 0.5), (double)x + 0.5);
			--chunk;
		}
		x = 0;
	}
}

int main(int argc, char* argv[]) {
	// base coordinates
	double y = 0.5, x = 0.5;

	// get config info
	int width = config::getInt("width");
	int height = config::getInt("height");
	int threadCount = config::getInt("threads");

	// init gui
	gui::setup();

	// pointer to seed object and parsing user input
	seed* s;
	std::string seedStr = config::getSeed();
	if (seedStr == "-1") {
		s = new seed();
		// write current seed to config file
		std::string generatedSeed = s->buildSeed();
		std::cout << "[+] Successfully generated seed:\n";
		std::cout << generatedSeed << '\n';
		config::setSeed(generatedSeed);
	} else {
		s = new seed(seedStr);
		if (!s->seedParsingSuccessful) {
			std::cout << "[-] Input seed not valid. Exiting.\n";
			delete s;
			return 0;
		}
	}

	// pointer to fractal object
	fractal* f = new fractal(s);

	// pointer to renderer object
	renderer* r = new renderer(width, height, s, f);

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
			threads.push_back(std::thread{renderRange, lastY, y, lastX, x, width, height, chunk, r, image});
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
		threads.push_back(std::thread{renderRange, lastY, height, lastX, width, width, height, chunk, r, image});
	}

	//
	// inform that everything's alright
	//
	std::cout << "[+] Rendering seeded region:\n";

	// render the first chunk in the main application thread
	// so that it can update the gui progress bar
	int count = 0;
	std::thread{gui::update, &count, chunk}.detach();
	for (int y = 0; y < height && count < chunk; ++y) {
		for (int x = 0; x < width && count < chunk; ++x) {
			(*image)[y][x] = r->render((double)height - ((double)y + 0.5f), (double)x + 0.5);
			++count;
		}
	}

	// wait for rendering threads to finish
	for (auto& thread : threads) {
		thread.join();
	}

	//
	// free heap allocated memory
	//
	delete s;
	delete f;
	delete r;

	//
	// define output file's path
	//
	int pathCount = 0;
	std::string path;
	for (bool ok = true; ok & 1; ++pathCount) {
		path = "render" + std::to_string(pathCount);
		std::ifstream pngFile(path + ".png");
		std::ifstream ppmFile(path + ".ppm");
		ok = pngFile.good() || ppmFile.good(); 
	}

	if (config::getInt("png")) {
		//
		// convert to portable network graphics format.
		// all credits for this functionality go to Nayuki for writing
		// the Tiny PNG Output library:
		// https://www.nayuki.io/page/tiny-png-output
		//
		try {
			std::ofstream out(path + ".png", std::ios::binary);
			TinyPngOut pngout(static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), out);
			std::vector<std::uint8_t> line(static_cast<size_t>(width) * 3);
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					math::vec3 pixel = (*image)[y][x];
					line[x * 3 + 0] = static_cast<uint8_t>(pixel.x);
					line[x * 3 + 1] = static_cast<uint8_t>(pixel.y);
					line[x * 3 + 2] = static_cast<uint8_t>(pixel.z);
				}
				pngout.write(line.data(), static_cast<size_t>(width));
			}
			std::cout << "[+] Successfully rendered region to " << path << ".png\n\n";	
		} catch (const char* message) {
			std::cout << message << std::endl;
		}
	} else {
		//
		// write image data to ppm file
		//
		std::ofstream out(path + ".ppm");
		out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				math::vec3 pixel = (*image)[i][j];
				out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
			}
		}
		std::cout << "[+] Successfully rendered region to " << path << ".ppm\n\n";
		return 0;
	}

	// free the last pointer storing the image pixels
	delete image;

	// correct teminal color pallette
	std::cout << "\033[0m";

	// exit :D
	return 0;
}
