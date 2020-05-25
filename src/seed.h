#pragma once

#include "math.h"

#include <string>
#include <vector>
#include <random>
#include <map>

class seed {
	private:
		std::random_device dev;
		std::mt19937 rng;
		math::vec3 vec3(double min, double max);

	public:
		// used to catch any exceptions when parsing a user
		// input seed
		bool seedParsingSuccessful;

		seed();
		// build 'values' dictionary from a user input seed.
		// seed operators:
		// ~start/end operators: { {, (, <, [ }
		// ~separation operators: { @, #, $, %, ^, &, * }
		// seed structure:
		// [ gloss_chance, gloss_amount, xcamdir, ycamdir,
		// zcamdir, camdist, xlightdir, ylightdir, zlightdir,
		// xlightcol, ylightcol, zlightcol, xskycol, yskycol,
		// zskycol, iter, colorRange, xcol, ycol, zcol, xgt,
		// ygt, zgt, xgb, ygb, zgb, xshift, zshift,
		// xrot, zrot, shadow_softness ]
		seed(std::string s);
		~seed();
	
		// dictionary for constants and their values
		std::map<std::string, double> values;

		// get the current seed based off the values dictionary
		std::string buildSeed();

		// get random integer in range between min and max
		int i(int min, int max);

		// get random double in range between min and max
		double d(double min, double max);
};
