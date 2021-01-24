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
#include <iostream>
#include <map>
#include <queue>
#include <ft2build.h>
#include FT_FREETYPE_H 

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
		bool _initialized = false;
		GLuint _vao;
		GLuint _tile_pos_vbo;
		GLuint _tile_tex_vbo;
		GLuint _text_vbo;
		unsigned int _shader_program_id;
		unsigned int _position_attrib_location;
		unsigned int _texture_coord_attrib_location;
		glm::mat4 _transform;
		glm::vec2 _focused_tile_position = INIT_FOCUSED_TILE_POSITION;
		glm::vec2 _boundary_pos = { 0, 0 };
		int _row_to_tiles_frame[MAX_SETS_RENDERED][MAX_TILES_RENDERED];
		int _row_indices[MAX_SETS_RENDERED];
		bool _shift_tiles_horizontal = false;
		bool _shift_tiles_vertical = false;
		int _shift_y_offset = 0;
		int _shift_x_offset = 0;
		std::map<GLchar, Character> _characters;
		std::queue<Ref_Set_Info> _ref_sets_info;

		void init();
		void load_textures();
		void load_homepage_api_json();
		bool init_text_dependencies();
		std::unique_ptr<Texture> download_texture(const char*);
		void check_for_horizontal_boundary_hit(const glm::vec2& pos);
		void check_for_vertical_boundary_hit(const glm::vec2& pos);
		void load_all_reference_sets();
		void load_reference_set(const Ref_Set_Info&);
		void render_text(std::string text, float x, float y, float scale, glm::vec3 color);
		bool consume_ref_set();

	public:
		Renderer(unsigned int shader_program_id,
			unsigned int position_attrib_location,
			unsigned int texture_coord_attrib_location);

		~Renderer();
		bool get_is_initialized() const { return _initialized; }
		void draw_home_page();
		void process_controller_input(const ControllerInput);
		inline GLuint get_vao() const { return _vao; }
	};
}
#endif
