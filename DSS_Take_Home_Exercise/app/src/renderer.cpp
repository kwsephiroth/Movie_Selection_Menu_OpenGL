#include "renderer.h"
#include <iostream>//temp

namespace DSS
{
	Renderer::Renderer(unsigned int shader_program_id,
		unsigned int position_attrib_location,
		unsigned int texture_coordinate_attrib_location) :
		_shader_program_id(shader_program_id),
		_position_attrib_location(position_attrib_location),
		_texture_coord_attrib_location(texture_coordinate_attrib_location)
	{
		init();

		//Initialize tiles frame array
		for (int i = 0; i < MAX_SETS_RENDERED; ++i)//TODO: Make row count a constant
		{
			for (int j = 0; j < MAX_TILES_RENDERED; ++j)//TODO: Make column count a constant
			{
				_row_to_tiles_frame[i][j] = j;
			}
		}
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::init()
	{
		std::cout << "Initializing home page ...." << std::endl;
		//TODO: Determine if this function should throw an exception if it fails to initialize the object state.
		load_homepage_api_json();
		if (_home_json_ptr)
		{
			//Load objects needed for rendering text.
			//TODO: Implement text rendering.
			//assert(init_text_dependencies());

			//Generate Vertex Array Object
			glGenVertexArrays(1, &_vao);
			glBindVertexArray(_vao);
			
			load_textures();
			//init_meshes();
			
			float tile_positions[12] = {
			   -0.5f,  -0.5f, 0.0f, //lower left
				0.5f,  -0.5f, 0.0f, //lower right
				0.5f, 0.5f, 0.0f,   //upper right
			   -0.5f, 0.5f, 0.0f    //upper left
			};

			float tile_tex_coords[8] = {
				0.0f, 1.0f,
				1.0f, 1.0f,
				1.0f, 0.0f,
				0.0f, 0.0f
			};
			
			//Generate Vertex Buffer Objects
			glGenBuffers(1, &_tile_pos_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _tile_pos_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tile_positions), tile_positions, GL_STATIC_DRAW);

			glGenBuffers(1, &_tile_tex_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _tile_tex_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tile_tex_coords), tile_tex_coords, GL_STATIC_DRAW);

			glBindVertexArray(0);
		}
		std::cout << "Home page initialized!" << std::endl;
		std::cout << "Rendering home page ... " << std::endl;
	}

	bool Renderer::init_text_dependencies()
	{
		if (FT_Init_FreeType(&_ft)) {
			fprintf(stderr, "Could not init freetype library\n");
			return false;
		}

		if (FT_New_Face(_ft, "./app/res/fonts/FreeSans.ttf", 0, &_face)) //TODO: copy the file to executable directory
		{
			fprintf(stderr, "Could not open font\n");
			return false;
		}

		FT_Set_Pixel_Sizes(_face, 0, 48);

		if (FT_Load_Char(_face, 'X', FT_LOAD_RENDER)) {
			fprintf(stderr, "Could not load character 'X'\n");
			return false;
		}

		_glyph_slot = _face->glyph;

		return true;
	}

	void Renderer::load_homepage_api_json()
	{
		//Attempt to download api json using url
		{
			auto file_memory_ptr = curl_utils::download_file_to_memory(HOME_JSON_URL);
			_home_json_ptr = Utils::get_rj_document(file_memory_ptr->memory);
			assert(_home_json_ptr);//TODO: Remove this assert for a better option
		}

		const auto containers_arr_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer("/data/StandardCollection/containers"));
		if (containers_arr_ptr && !containers_arr_ptr->IsNull())
		{
			if (containers_arr_ptr->IsArray())
			{
				const auto& containers_arr = containers_arr_ptr->GetArray();
				for (size_t i = 0; i < containers_arr.Size(); ++i)//Iterate over outer most sets
				{
					std::string set_name_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/text/title/full/set/default/content";
					const auto set_name_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(set_name_path.c_str()));
					if (!set_name_ptr || set_name_ptr->IsNull() || !set_name_ptr->IsString())
						continue;//skip rest of iteration

					Set dss_set;
					dss_set.name = set_name_ptr->GetString();

					//std::cout << "set name = " << set_name_ptr->GetString() << std::endl;

					std::string items_arr_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/items";
					const auto items_arr_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(items_arr_path.c_str()));
					if (items_arr_ptr && !items_arr_ptr->IsNull())
					{
						if (items_arr_ptr->IsArray())
						{
							/*total_items += items_arr_ptr->GetArray().Size();*/
							const auto& items_arr = items_arr_ptr->GetArray();
							for (size_t j = 0; j < items_arr.Size(); ++j)
							{
								std::string full_image_data_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/items/" + std::to_string(j) +
									"/image/tile/1.78";//TODO: Determine what the integer member of the "tile" object represents.
								
								auto image_data_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));

								if (image_data_ptr && !image_data_ptr->IsNull())
								{
									std::string item_type = image_data_ptr->MemberBegin()->name.GetString();
									full_image_data_path += ("/" + item_type + "/default");
									image_data_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));
									if (image_data_ptr && !image_data_ptr->IsNull())
									{
										//TODO: Store image url, width, and height
										if (image_data_ptr->IsObject())
										{
											//TODO: Determine what happens if these members don't exist
											const auto image_url_ptr = image_data_ptr->FindMember("url");
											const auto image_width_ptr = image_data_ptr->FindMember("masterWidth");
											const auto image_height_ptr = image_data_ptr->FindMember("masterHeight");

											if (!image_url_ptr->value.IsString() || !image_width_ptr->value.IsInt() || !image_height_ptr->value.IsInt())
												continue;

											dss_set.tiles.push_back({image_url_ptr->value.GetString(), 
												image_width_ptr->value.GetInt(),
												image_height_ptr->value.GetInt(), 
												nullptr });

										}
									}
								}
							}
						}
						_sets.push_back(std::move(dss_set));
					}
					else//This may be a ref set so check for ref id and construct ref set url.
					{
						std::string ref_id_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/refId";
						const auto ref_id_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(ref_id_path.c_str()));
						
						if (!ref_id_ptr || ref_id_ptr->IsNull() || !ref_id_ptr->IsString())
							continue;

						Ref_Set_Info ref_set;
						ref_set.name = std::move(dss_set.name);
						ref_set.ref_set_url = DSS::REF_SETS_URL_PREFIX + std::string(ref_id_ptr->GetString()) + ".json";
						_ref_sets_info.push_back(std::move(ref_set));
					}
				}//sets
			}
		}
	}

	void Renderer::load_textures()
	{
		for (auto& set : _sets)
		{	
			//for (auto& tile : set.tiles)
			for(auto itr = set.tiles.begin(); itr != set.tiles.end();)
			{
				auto texture_ptr = download_texture(itr->image_url.c_str());
				if (!texture_ptr)//Skip any textures that couldn't be successfully downloaded.
				{
					itr = set.tiles.erase(itr);
				}
				else
				{
					itr->texture = std::move(texture_ptr);
					++itr;
				}
			}
		}
	}

	std::unique_ptr<Texture> Renderer::download_texture(const char * img_url)
	{

		//std::string out_file_name = "image" + std::to_string(j) + ".jpeg";
		auto file_memory_ptr = curl_utils::download_file_to_memory(img_url);
		if (file_memory_ptr)
		{
			int width = 0;
			int height = 0;
			int channels = 0;
			std::unique_ptr<unsigned char> image_buffer_ptr(SOIL_load_image_from_memory((const unsigned char*)file_memory_ptr->memory,
				file_memory_ptr->size,
				&width,
				&height,
				&channels,
				SOIL_LOAD_AUTO));

			if (!image_buffer_ptr)
			{
				return nullptr;
			}
			//SOIL_save_image(out_file_name.c_str(), SOIL_SAVE_TYPE_JPG, width, height, channels, image_buffer_ptr.get());
			return std::unique_ptr<Texture>(new Texture(std::move(image_buffer_ptr), width, height));
		}
		return nullptr;
	}

	void Renderer::load_all_reference_sets()
	{
		for (const auto& ref_set_info : _ref_sets_info)
		{
			load_reference_set(ref_set_info);
		}
	}

	void Renderer::load_reference_set(const Ref_Set_Info& ref_set_info)
	{
		auto file_memory_ptr = curl_utils::download_file_to_memory(ref_set_info.ref_set_url.c_str());
		auto ref_set_json_ptr = Utils::get_rj_document(file_memory_ptr->memory);
		
		if (!ref_set_json_ptr) //Something went wrong trying to fetch this ref set so give up
			return;

		//Parse ref set json and append to sets collection
		Set dss_set;
		dss_set.name = ref_set_info.name;

		std::string items_arr_path = "/data/CuratedSet/items/";
		const auto items_arr_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(items_arr_path.c_str()));
		if (items_arr_ptr && !items_arr_ptr->IsNull())
		{
			if (items_arr_ptr->IsArray())
			{
				const auto& items_arr = items_arr_ptr->GetArray();
				for (size_t j = 0; j < items_arr.Size(); ++j)
				{
					std::string full_image_data_path = items_arr_path + std::to_string(j) +
						"/image/tile/1.78";//TODO: Determine what the integer member of the "tile" object represents.

					auto image_data_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));

					if (image_data_ptr && !image_data_ptr->IsNull())
					{
						std::string item_type = image_data_ptr->MemberBegin()->name.GetString();
						full_image_data_path += ("/" + item_type + "/default");
						image_data_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));
						if (image_data_ptr && !image_data_ptr->IsNull())
						{
							//TODO: Store image url, width, and height
							if (image_data_ptr->IsObject())
							{
								//TODO: Determine what happens if these members don't exist
								const auto image_url_ptr = image_data_ptr->FindMember("url");
								const auto image_width_ptr = image_data_ptr->FindMember("masterWidth");
								const auto image_height_ptr = image_data_ptr->FindMember("masterHeight");

								if (!image_url_ptr->value.IsString() || !image_width_ptr->value.IsInt() || !image_height_ptr->value.IsInt())
									continue;

								dss_set.tiles.push_back({ image_url_ptr->value.GetString(),
									image_width_ptr->value.GetInt(),
									image_height_ptr->value.GetInt(),
									nullptr });

							}
						}
					}
				}
			}
			_sets.push_back(std::move(dss_set));
		}
	}

	void Renderer::draw_home_page()
	{
		glUseProgram(_shader_program_id);

		static const float SCALE_FACTOR = 0.3f;
		static const float POS_OFFSET_X = -.8f;
		static const float POS_OFFSET_Y = .7f;
		static const float FOCUSED_SCALE_FACTOR = 0.39f;

		glBindVertexArray(_vao);

//DRAW TILE GRID
		float spacing_update_x = 0;
		float spacing_update_y = 0;
		size_t rendered_tile_count = 0;
		size_t rendered_row_count = 0;
		size_t row_index = 0;
		size_t row_pos = 0;

		while (rendered_row_count < MAX_SETS_RENDERED && row_index < _sets.size())
		{
			row_pos = row_index;

			//Process any tile shifting here
			//Check tile frame for any invalid indices
			bool invalid_tiles_frame = false;
			for (int tile_index = 0; tile_index < MAX_TILES_RENDERED; ++tile_index)
			{
				int current_index = _row_to_tiles_frame[row_index][tile_index];
				if (current_index < 0 || current_index >= _sets[row_index].tiles.size())//INVALID FRAME DETECTED
				{
					invalid_tiles_frame = true;
					break;
				}
			}

			if (invalid_tiles_frame)
				continue;//Skip rendering for this row


			for(int tile_index = 0; tile_index < MAX_TILES_RENDERED; ++tile_index)
			{
				if (tile_index >= _sets[row_index].tiles.size())
					break;

				int frame_index = _row_to_tiles_frame[row_index][tile_index];
				auto& current_tile = _sets[row_index].tiles[frame_index];
				current_tile.position = { row_pos, tile_index };
				//std::cout << "current_tile.position = (" << current_tile.position.x << " , " << current_tile.position.y << " )" << std::endl;

				//Apply any transformations to tiles
				glm::mat4 transform(1);//Initialize to identity matrix
				transform = glm::translate(transform, glm::vec3(POS_OFFSET_X + spacing_update_x, POS_OFFSET_Y + spacing_update_y, 0.0f));
				
				if (current_tile.position == _focused_tile_position)//Apply additional scaling to focused tile
				{
					current_tile.is_focused = true;
					transform = glm::scale(transform, glm::vec3(FOCUSED_SCALE_FACTOR, FOCUSED_SCALE_FACTOR, 0.0f));
				}
				else
				{
					current_tile.is_focused = false;
					transform = glm::scale(transform, glm::vec3(SCALE_FACTOR, SCALE_FACTOR, 0.0f));
				}

				GLuint transLoc = glGetUniformLocation(_shader_program_id, "transform");
				glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
				//

				glBindBuffer(GL_ARRAY_BUFFER, _tile_pos_vbo);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, _tile_tex_vbo);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				current_tile.texture->bind(0);
				glDrawArrays(GL_QUADS, 0, 4);
				current_tile.texture->unbind();

				//update counters
				spacing_update_x += 0.37f;
			}

			spacing_update_x = 0;
			spacing_update_y -= 0.5f;
			++rendered_row_count;
			++row_index;
		}
//END DRAW TILE GRID

		glBindVertexArray(0);
	}

	void Renderer::process_controller_input(const ControllerInput input)
	{
		switch (input)
		{
			case ControllerInput::UP:
			{
				if (_focused_tile_position.x == SETS_UPPER_BOUNDARY_X)
				{
					//std::cout << "Can't move UP any further." << std::endl;
					check_for_vertical_boundary_hit(_focused_tile_position);
				}
				else
				{
					--_focused_tile_position.x;
				}
			}
			break;

			case ControllerInput::DOWN:
			{
				if (_focused_tile_position.x == SETS_LOWER_BOUNDARY_X)
				{
					//std::cout << "Can't move DOWN any further." << std::endl;
					check_for_vertical_boundary_hit(_focused_tile_position);
				}
				else
				{
					++_focused_tile_position.x;
				}
			}
			break;

			case ControllerInput::LEFT:
			{
				if (_focused_tile_position.y == TILES_LEFT_BOUNDARY_Y)
				{
					//std::cout << "Can't move LEFT any further." << std::endl;
					check_for_horizontal_boundary_hit(_focused_tile_position);
				}
				else
				{
					--_focused_tile_position.y;
				}
			}
			break;

			case ControllerInput::RIGHT:
			{
				if (_focused_tile_position.y == TILES_RIGHT_BOUNDARY_Y)
				{
					//std::cout << "Can't move RIGHT any further." << std::endl;
					check_for_horizontal_boundary_hit(_focused_tile_position);
				}
				else
				{
					++_focused_tile_position.y;
				}
			}
			break;

			//case ControllerInput::ENTER:
			//{

			//}
			//break;

			//case ControllerInput::BACK:
			//{

			//}
			//break;

			default:
			{
				std::cerr << "ERROR: Invalid controller input detected." << std::endl;
				return;
			}
		}

		std::cout << "focused_tile_position (" << _focused_tile_position.x << " , " << _focused_tile_position.y << " ) " << std::endl;
	}

	void Renderer::check_for_horizontal_boundary_hit(const glm::vec2& pos)
	{
		if (pos.y == TILES_LEFT_BOUNDARY_Y)
		{
			//std::cout << "Horizontal Boundary Hit Detected!" << std::endl;
			//shift tiles right
			if (_row_to_tiles_frame[(int)pos.x][0] == TILES_LEFT_BOUNDARY_Y)//DON'T UPDATE FRAME!!!
				return;

			for (int tile_index = 0; tile_index < MAX_TILES_RENDERED; ++tile_index)
			{
				int new_index = _row_to_tiles_frame[(int)pos.x][tile_index] - 1;
				if (new_index < 0)//new index out of range of current row//DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
					_row_to_tiles_frame[(int)pos.x][tile_index] = new_index;
				}
			}
		}
		else if (pos.y == TILES_RIGHT_BOUNDARY_Y)
		{
			//std::cout << "Horizontal Boundary Hit Detected!" << std::endl;

			auto current_row_tile_count = _sets[(int)pos.x].tiles.size();

			//shift tiles left
			if (_row_to_tiles_frame[(int)pos.x][MAX_TILES_RENDERED - 1] == (current_row_tile_count - 1))//DON'T UPDATE FRAME!!!
				return;

			for (int tile_index = 0; tile_index < MAX_TILES_RENDERED; ++tile_index)
			{
				int new_index = _row_to_tiles_frame[(int)pos.x][tile_index] + 1;
				if (new_index >= current_row_tile_count)//new index out of range of current row //DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
				  _row_to_tiles_frame[(int)pos.x][tile_index] = new_index;
				}
			}
		}
	}

	void Renderer::check_for_vertical_boundary_hit(const glm::vec2& pos)
	{
		if (pos.x == SETS_UPPER_BOUNDARY_X)
		{
			//std::cout << "Vertical Boundary Hit Detected!" << std::endl;
			//shift tiles down
		}
		else if (pos.x == SETS_LOWER_BOUNDARY_X)
		{
			//std::cout << "Vertical Boundary Hit Detected!" << std::endl;
			//shift tiles up
		}
	}
}