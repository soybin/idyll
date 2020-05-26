/*
 * MIT License
 * Copyright (c) 2020 Pablo Peñarroja
 */

// png lib by Nayuki (https://www.nayuki.io/page/tiny-png-output)
#include "TinyPngOut.hpp"

#include "config.h"
#include "fractal.h"
#include "gui.h"
#include "math.h"
#include "seed.h"
#include "renderer.h"

#include <fstream>
#include <iostream>
#include <thread>

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
	if (argc > 2) {
		std::cout << "[-] The only permissible argument is a seed's file location.\n\n";
		return 0;
	} else if (argc == 2) {
		std::ifstream seedFile(argv[1]);
		if (!seedFile.good()) {
			std::cout << "[-] Couldn't find specified file.\n\n";
			return 0;
		}
		std::string seedData;
		std::getline(seedFile, seedData);
		s = new seed(seedData);
		if (!s->seedParsingSuccessful) {
			std::cout << "[-] Input seed not valid.\n\n";
			delete s;
			return 0;
		}
		std::cout << "[+] Successfully read seed from '" << argv[1] << "'.\n";
	} else {
		s = new seed();
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
	// define output file's path
	//
	int fileCount = 0;
	std::string fileCountStr;
	for (bool ok = true; ok & 1; ++fileCount) {
		fileCountStr = std::to_string(fileCount);
		std::ifstream pngFile("render" + fileCountStr + ".png");
		std::ifstream ppmFile("render" + fileCountStr + ".ppm");
		ok = pngFile.good() || ppmFile.good(); 
	}

	//
	// store seed inside new file
	//
	std::ofstream seedOut("seed" + fileCountStr + ".txt");
	seedOut << s->buildSeed();
	std::cout << "[+] Successfully stored seed at 'seed" << fileCountStr << ".txt'.\n";

	if (config::getInt("png")) {
		//
		// convert to portable network graphics format.
		// all credits for this functionality go to Nayuki for writing
		// the Tiny PNG Output library:
		// https://www.nayuki.io/page/tiny-png-output
		//
		try {
			// write image
			std::ofstream out("render" + fileCountStr + ".png", std::ios::binary);
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
			std::cout << "[+] Successfully stored png file to 'render" << fileCountStr << ".png'.\n\n";	
		} catch (const char* message) {
			std::cout << message << std::endl;
		}
	} else {
		//
		// write image data to ppm file
		//
		std::ofstream out("render" + fileCountStr + ".ppm");
		out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				math::vec3 pixel = (*image)[i][j];
				out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
			}
		}
		std::cout << "[+] Successfully store ppm file to 'render" << fileCountStr << ".ppm'.\n\n";
	}

	//
	// free heap allocated memory
	//
	delete s;
	delete f;
	delete r;
	delete image;

	// correct teminal color pallette
	std::cout << "\033[0m";

	// exit :D
	return 0;
}
