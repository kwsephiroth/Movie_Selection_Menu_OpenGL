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

	struct Vertex
	{
		GLfloat position[2];//x,y
		GLfloat textureCoordinates[2];//u,v
	};

	class Renderer//TODO: Determine if this will need to be a singleton class.
	{
	private:
		std::unique_ptr<rapidjson::Document> _home_json_ptr;//TODO: Consider having this dependency injected through constructor.
		std::vector<Set> _sets;
		std::vector<Ref_Set_Info> _ref_sets_info;
		bool _initialized = false;
		Vertex _tile_vertices[4];
		GLuint _vao;
		GLuint _tile_vbo;
		unsigned int _shader_program_id;
		unsigned int _position_attrib_location;
		unsigned int _texture_coord_attrib_location;

		void init(unsigned int shader_program_id,
			unsigned int position_attrib_location,
			unsigned int texture_coordinate_attrib_location);

		void load_textures();
		void load_homepage_api_json();
		void init_tile_vertices();
		void init_menu_grid();
		void setup_vao(const GLuint position_attrib_location, const GLuint texture_attrib_location);

	public:
		Renderer(unsigned int shader_program_id,
			unsigned int position_attrib_location,
			unsigned int texture_coordinate_attrib_location);

		~Renderer();
		bool get_is_initialized() const { return _initialized; }
		void draw_menu();
		GLuint get_vao() const { return _vao; }
	};
}
#endif
