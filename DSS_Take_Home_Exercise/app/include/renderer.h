#ifndef RENDERER_H_
#define RENDERER_H_
#include "utils.h"
#include <rapidjson/document.h>
#include <memory>
#include <SOIL2/SOIL2.h>
#include "curl_utils.h"
#include "constants.h"
#include "rapidjson/pointer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "texture.h"

namespace DSS
{
	struct Tile
	{
		std::string image_url;
		int image_width;
		int image_height;
		std::unique_ptr<Texture> texture = nullptr;
	};
	
	struct Set
	{
		std::string name;
		std::vector<Tile> tiles;
	};

	struct Ref_Set_Info
	{
		std::string name;
		std::string ref_set_url;
	};

	class Renderer//TODO: Determine if this will need to be a singleton class.
	{
	private:
		std::unique_ptr<rapidjson::Document> _home_json_ptr;//TODO: Consider having this dependency injected through constructor.
		std::vector<Set> _sets;
		std::vector<Ref_Set_Info> _ref_sets_info;
		bool _initialized = false;

		void init();
		void load_textures();
		void load_homepage_api_json();

	public:
		Renderer();
		bool get_is_initialized() const { return _initialized; }
	};
}
#endif
