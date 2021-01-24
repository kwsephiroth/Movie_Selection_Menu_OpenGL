#ifndef RENDERER_H_
#define RENDERER_H_
#include <rapidjson/document.h>
#include <memory>
#include <SOIL2/SOIL2.h>
#include "constants.h"
#include "rapidjson/pointer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture.h"
#include "utils.h"
#include "curl_utils.h"
#include "text.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <unordered_map>

namespace DSS
{
	struct Tile
	{
		std::string image_url;
		int master_width;
		int master_height;		
		std::unique_ptr<Texture> texture = nullptr;
		bool is_focused = false;
		bool in_view = false;
		glm::vec2 position;

		void update_in_view()
		{
			//std::cout << " ( " << position.x << " , " << position.y << " )" << std::endl;
			if (position.x >= 0 && position.x <= 3)
			{
				if (position.y >= 0 && position.y <= 4)
				{
					in_view = true;
					return;
				}
			}
			in_view = false;
		}
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
		glm::vec3 position;
		glm::vec2 texCoord;
	};

	enum class ControllerInput
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		ENTER,
		BACK
	};

	class Renderer//TODO: Determine if this will need to be a singleton class.
	{
	private:
		std::unique_ptr<rapidjson::Document> _home_json_ptr;//TODO: Consider having this dependency injected through constructor.
		std::vector<Set> _sets;
		std::vector<Ref_Set_Info> _ref_sets_info;
		bool _initialized = false;
		GLuint _vao;
		GLuint _tile_pos_vbo;
		GLuint _tile_tex_vbo;
		//Vertex _tile_vertices[4];
		unsigned int _shader_program_id;
		unsigned int _position_attrib_location;
		unsigned int _texture_coord_attrib_location;
		glm::mat4 _transform;
		glm::vec2 _focused_tile_position = INIT_FOCUSED_TILE_POSITION;
		FT_Library _ft;
		FT_Face _face;
		FT_GlyphSlot _glyph_slot;
		glm::vec2 _boundary_pos = { 0, 0 };
		unsigned int _row_to_tiles_frame[4][5];
		bool _shift_tiles_horizontal = false;
		bool _shift_tiles_vertical = false;
		int _shift_y_offset = 0;
		int _shift_x_offset = 0;

		void init();
		void load_textures();
		void load_homepage_api_json();
		void init_meshes();
		void init_menu_grid();
		bool init_text_dependencies();
		std::unique_ptr<Texture> download_texture(const char*);
		bool check_for_horizontal_boundary_hit(const glm::vec2& pos);
		bool check_for_vertical_boundary_hit(const glm::vec2& pos);

	public:
		Renderer(unsigned int shader_program_id,
			unsigned int position_attrib_location,
			unsigned int texture_coord_attrib_location);

		~Renderer();
		bool get_is_initialized() const { return _initialized; }
		void draw_home_page();
		void process_controller_input(const ControllerInput);// , const glm::vec2&);

		GLuint get_vao() const { return _vao; }
	};
}
#endif
