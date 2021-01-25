#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <glm/glm.hpp>

namespace DSS
{
	static const char * HOME_JSON_URL = "https://cd-static.bamgrid.com/dp-117731241344/home.json";
	static const char* REF_SETS_URL_PREFIX = "https://cd-static.bamgrid.com/dp-117731241344/sets/";
	static const glm::vec2 INIT_FOCUSED_TILE_POSITION = glm::vec2(0, 0);
	static const unsigned int MAX_COLUMNS_RENDERED = 6;
	static const unsigned int MAX_ROWS_RENDERED = 4;
	static const unsigned int COLUMNS_LEFT_BOUNDARY_Y = 0;
	static const unsigned int COLUMNS_RIGHT_BOUNDARY_Y = (MAX_COLUMNS_RENDERED - 2);
	static const unsigned int ROWS_UPPER_BOUNDARY_X = 0;
	static const unsigned int ROWS_LOWER_BOUNDARY_X = (MAX_ROWS_RENDERED - 1);
}
#endif
