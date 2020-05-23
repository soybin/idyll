#include <algorithm>
#include "seed.h"

// mersennes' twister prng algo initialized with random device
// seed

seed::seed() : rng(dev()) {
	//                                                      //
	//======== r e n d e r e r    c o n s t a n t s ========//
	//                                                      //
	// chance for a ray to be reflected with a cone distribution
	values["GLOSSINESS_CHANCE"] = d(0.5, 1.0);
	// how glossy should the reflection be
	values["GLOSSINESS_AMOUNT"] = d(0.0, 0.5);
	// direction in which a ray should be marched until getting
	// out of the fractal distance estimator
	math::vec3 cameraDirection = math::normalize(vec3(-1.0, 1.0));
	values["xcameraDirection"] = cameraDirection.x;
	values["ycameraDirection"] = cameraDirection.y;
	values["zcameraDirection"] = cameraDirection.z;
	// how far away should the camera be from the surface of the
	// distance estimator
	values["cameraDistance"] = i(3, 9);
	// directional light direction
	math::vec3 lightDirection = math::normalize(cameraDirection + vec3(-0.01, 0.01));
	values["xlightDirection"] = lightDirection.x;
	values["ylightDirection"] = lightDirection.y;
	values["zlightDirection"] = lightDirection.z;
	// directional light color
	math::vec3 lightColor;
	lightColor.x = d(0.9, 1.1);
	lightColor.y = d(lightColor.x - 0.1, lightColor.x + 0.1);
	lightColor.z = d(lightColor.x - 0.2, lightColor.x + 0.2);
	lightColor = math::normalize(lightColor);
	values["xlightColor"] = lightColor.x;
	values["ylightColor"] = lightColor.y;
	values["zlightColor"] = lightColor.z;
	// sky color
	math::vec3 skyColor;
	skyColor.x = d(0.75, 1.0);
	skyColor.y = d(skyColor.x - 0.125, skyColor.x + 0.125);
	skyColor.z = d(skyColor.x - 0.125, skyColor.x + 0.25);
	skyColor = math::normalize(skyColor);
	values["xskyColor"] = skyColor.x;
	values["yskyColor"] = skyColor.y;
	values["zskyColor"] = skyColor.z;

	//                                                    //
	//======== f r a c t a l    c o n s t a n t s ========//
	//                                                    //
	// number of iterations
	values["iterations"] = i(16, 20);
	// fractal base color
	math::vec3 color;
	color.x = d(0.0, 1.0);
	color.z = d(0.0, 1.0 - color.x);
	color.y = d(0.0, 1.0 - color.x - color.z);
	color = math::normalize(math::vec3(0.4) + color);
	values["xcolor"] = color.x;
	values["ycolor"] = color.y;
	values["zcolor"] = color.z;
	// top sky gradient color
	math::vec3 normalizedGradientTop = math::normalize(vec3(0, 255));
	values["xgradientTop"] = normalizedGradientTop.x;
	values["ygradientTop"] = normalizedGradientTop.y;
	values["zgradientTop"] = normalizedGradientTop.z;
	// bottom sky gradient color
	math::vec3 normalizedGradientBottom = math::normalize(vec3(0, 255));
	values["xgradientBottom"] = normalizedGradientBottom.x;
	values["ygradientBottom"] = normalizedGradientBottom.y;
	values["zgradientBottom"] = normalizedGradientBottom.z;
	// fractal point space shift per iteration
	double maxShift = 0.6;
	math::vec3 shift = vec3(-maxShift, -maxShift / 32.0);
	values["xshift"] = shift.x;
	values["yshift"] = shift.y / 16.0;
	values["zshift"] = shift.z;	
	// fractal point space rotation per iteration
	double maxRotation = 0.2;
	math::vec3 rotation = vec3(-maxRotation, -maxRotation / 32.0);
	values["xrotation"] = rotation.x;
	values["yrotation"] = rotation.y / 16.0;
	values["zrotation"] = rotation.z;
	// distancce estimator scale
	values["deScale"] = d(1.0, 1.0);
	// shadow softess
	values["shadowSoftness"] = d(0.5, 1.0);
}

seed::seed(std::string s) {
	seedParsingSuccessful = true;
	std::vector<char> startOps = { '{', '(', '<', '[' };
	std::vector<char> endOps = { '}', ')', '>', ']' };
	std::vector<char> separationOps = { '!', '@', '#', '$', '%', '^', '&', '*' };
	int n = s.size();
	std::string str = "";
	for (int i = 0, arg = 0; i < n; ++i) {
		char c = s[i];
		bool end = false;
		for (int j = 0; j < 4; ++j) {
			if (c == endOps[j]) {
				end = true;
				break;
			}
		}
		if ((c == '.' || c == '-' || (c >= '0' && c <= '9')) && !end) {
			str += c;
		} else if (str.size() || end) {
			//str += c;
			double value;
			try {
				value = std::stod(str);
			} catch (...) {
				seedParsingSuccessful = false;
				return;
			}
			switch (arg) {
				case 0:
					values["GLOSSINESS_CHANCE"] = value;
					break;
				case 1:
					values["GLOSSINESS_AMOUNT"] = value;
					break;
				case 2:
					values["xcameraDirection"] = value;
					break;
				case 3:
					values["ycameraDirection"] = value;
					break;
				case 4:
					values["zcameraDirection"] = value;
					break;
				case 5:
					values["cameraDistance"] = value;
					break;
				case 6:
					values["xlightDirection"] = value;
					break;
				case 7:
					values["ylightDirection"] = value;
					break;
				case 8:
					values["zlightDirection"] = value;
					break;
				case 9:
					values["xlightColor"] = value;
					break;
				case 10:
					values["ylightColor"] = value;
					break;
				case 11:
					values["zlightColor"] = value;
					break;
				case 12:
					values["xskyColor"] = value;
					break;
				case 13:
					values["yskyColor"] = value;
					break;
				case 14:
					values["zskyColor"] = value;
					break;
				case 15:
					values["iterations"] = value;
					break;
				case 16:
					values["xcolor"] = value;
					break;
				case 17:
					values["ycolor"] = value;
					break;
				case 18:
					values["zcolor"] = value;
					break;
				case 19:
					values["xgradientTop"] = value;
					break;
				case 20:
					values["ygradientTop"] = value;
					break;
				case 21:
					values["zgradientTop"] = value;
					break;
				case 22:
					values["xgradientBottom"] = value;
					break;
				case 23:
					values["ygradientBottom"] = value;
					break;
				case 24:
					values["zgradientBottom"] = value;
					break;
				case 25:
					values["xshift"] = value;
					break;
				case 26:
					values["yshift"] = value;
					break;
				case 27:
					values["zshift"] = value;
					break;
				case 28:
					values["xrotation"] = value;
					break;
				case 29:
					values["yrotation"] = value;
					break;
				case 30:
					values["zrotation"] = value;
					break;
				case 31:
					values["shadowSoftness"] = value;
			}
			++arg;
			str = "";
		}
	}
}

seed::~seed() {
}

std::string seed::buildSeed() {
	std::vector<char> startOps = { '{', '(', '<', '[' };
	std::vector<char> endOps = { '}', ')', '>', ']' };
	std::vector<char> separationOps = { '!', '@', '#', '$', '%', '^', '&', '*' };
	int n = startOps.size(), m = separationOps.size();
	std::string s = "";
	s += startOps[i(0, n - 1)];
	s += std::to_string(values["GLOSSINESS_CHANCE"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["GLOSSINESS_AMOUNT"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xcameraDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ycameraDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zcameraDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["cameraDistance"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xlightDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ylightDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zlightDirection"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xlightColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ylightColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zlightColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xskyColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["yskyColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zskyColor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["iterations"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xcolor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ycolor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zcolor"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xgradientTop"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ygradientTop"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zgradientTop"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xgradientBottom"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["ygradientBottom"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zgradientBottom"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xshift"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["yshift"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zshift"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["xrotation"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["yrotation"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["zrotation"]);
	s += separationOps[i(0, m)];
	s += std::to_string(values["shadowSoftness"]);
	s += endOps[i(0, n)];
	return s;
}

int seed::i(int min, int max) {
	std::uniform_int_distribution<int> dice(min, max);
	return dice(rng);
}

double seed::d(double min, double max) {
	std::uniform_real_distribution<double> dice(min, max);
	return dice(rng);
}

math::vec3 seed::vec3(double min, double max) {
	return math::vec3(d(min, max), d(min, max), d(min, max));
}
