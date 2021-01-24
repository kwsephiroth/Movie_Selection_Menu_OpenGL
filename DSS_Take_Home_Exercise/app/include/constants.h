#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <glm/glm.hpp>

namespace DSS
{
	static const char * HOME_JSON_URL = "https://cd-static.bamgrid.com/dp-117731241344/home.json";
	static const char* REF_SETS_URL_PREFIX = "https://cd-static.bamgrid.com/dp-117731241344/sets/";
	static const glm::vec2 INIT_FOCUSED_TILE_POSITION = glm::vec2(0, 0);
	static const int MAX_TILE_COUNT = 6;
	static const int MAX_SET_COUNT = 4;
	static const int TILES_LEFT_BOUNDARY_Y = 0;
	static const int TILES_RIGHT_BOUNDARY_Y = (MAX_TILE_COUNT - 2);
	static const int SETS_UPPER_BOUNDARY_X = 0;
	static const int SETS_LOWER_BOUNDARY_X = (MAX_SET_COUNT - 1);
}
#endif
