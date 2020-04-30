#include "config.h"
#include "fractal.h"
#include "gui.h"
#include "math.h"
#include "region.h"
#include <fstream>
#include <iostream>
#include <thread>

void renderThread(int startRow, int endRow, int startCol, int endCol, int width, int height, int chunk, fractal* f, std::vector<std::vector<math::vec3>>* image) {
	region reg(width, height, f);

	for (int y = startRow, x = startCol; y < height && (y < endRow || chunk > 0); ++y) {
		reg.setYCoord((float)height - ((float)y + 0.5f));
		for (; x < width && (x < endCol || chunk > 0); ++x) {
			reg.setXCoord((float)x + 0.5f);
			(*image)[y][x] = reg.render();
			--chunk;
		}
		x = 0;
	}
}

int main(int argc, char* argv[]) {
	// get config info
	int width = config::getInt("width");
	int height = config::getInt("height");
	if (width < 1 || height < 1) {
		return 0;
	}
 
	// coordinates
	float y = 0.5f, x = 0.5f;

	// gui
	gui::setup();

	// frac
	fractal* f = new fractal();
	//while (math::de::main(f->cam, f->rot, f->shift, 1.0f, f->iter) < 0.0f) {
		//f->cam += math::vec3(0.0f, 0.0f, 100.0f);
		//std::cout << f->cam.x << " " << f->cam.y << " " << f->cam.z << std::endl;
	//}

	f->cam = math::vec3(0.0f, 0.0f, 60.0f);
	// multithread	
	std::vector<std::vector<math::vec3>>* image = new std::vector<std::vector<math::vec3>>(height, std::vector<math::vec3>(width, math::vec3()));

	unsigned long long imageSize = width * height;
	unsigned long long thread_cnt = 1000;
	thread_cnt = std::min(thread_cnt, imageSize);
	std::vector<std::thread> threads;
	int chunk = imageSize / thread_cnt;

	int lastY = chunk / width - 1;
	int lastX = chunk % width - 1;

	for (int threadNum = 1; threadNum < thread_cnt; ++threadNum) {
		int y = lastY;
		int x = lastX;
		int count = 0;
		for (; x < width && count < chunk; ++x) {
			++count;
		}
		x = 0;
		for (; y < height && count < chunk; ++y) {
			for (; x < width && count < chunk; ++x) {
				++count;
			}
			if (count != chunk) x = 0;
		}
		threads.push_back(std::thread(renderThread, lastY, y, lastX, x, width, height, chunk, f, image));
		lastY = y;
		lastX = x;
	}

	// fill the remaining image portion which is smaller
	// than one chunk
	threads.push_back(std::thread(renderThread, lastY, height - 1, lastX, width - 1, width, height, chunk, f, image));

	// render the first chunk in this thread to update gui
	int count = 0;
	std::thread guiUpdate(gui::update, &count, chunk);
	region reg(width, height, f);
	for (int y = 0; y < height && count < chunk; ++y) {
		reg.setYCoord((float)height - ((float)y + 0.5f));
		for (int x = 0; x < width && count < chunk; ++x) {
			reg.setXCoord((float)x + 0.5f);
			(*image)[y][x] = reg.render();
			++count;
		}
	}
	guiUpdate.join();

	// wait for rendering threads to finish
	for (auto& thread : threads) {
		thread.join();
	}

	// write to output file
	std::ofstream out("out.ppm");
	out << "P3\n" << width << ' ' << height << ' ' << 255 << '\n';
	for (int i = 0; i < height; ++i) {
		for (const auto& pixel : (*image)[i]) {
			out << (int)pixel.x << ' ' << (int)pixel.y << ' ' << (int)pixel.z << '\n';
		}
	}

	// free memory
	delete f;
	delete image;

	return 0;
}
