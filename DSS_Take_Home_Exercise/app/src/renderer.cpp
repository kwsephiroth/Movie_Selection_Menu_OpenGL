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

		//Initialize row frame array
		for (int i = 0; i < MAX_ROWS_RENDERED; ++i)
		{
			_set_indices[i] = i;
		}

		//Initialize tiles frame array
		for (int i = 0; i < MAX_ROWS_RENDERED; ++i)//TODO: Make row count a constant
		{
			for (int j = 0; j < MAX_COLUMNS_RENDERED; ++j)//TODO: Make column count a constant
			{
				_tile_indices[i][j] = j;
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
			//Generate Vertex Array Object
			glGenVertexArrays(1, &_vao);

			//Load objects needed for rendering text.
			//TODO: Implement text rendering.
			assert(init_text_dependencies());

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
		FT_Library ft;

		if (FT_Init_FreeType(&ft)) {
			fprintf(stderr, "Could not init freetype library\n");
			return false;
		}

		FT_Face face;
		if (FT_New_Face(ft, "./app/res/fonts/FreeSans.ttf", 0, &face)) //TODO: copy the file to executable directory
		{
			fprintf(stderr, "Could not open font\n");
			return false;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				fprintf(stderr, "Could not load character %c\n", c);
				continue;
			}

			// generate texture for character
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			_characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);


		// configure VAO/VBO for texture quads
		// -----------------------------------
		//glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &_text_vbo);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _text_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		GLuint char_attrib_location = glGetAttribLocation(_shader_program_id, "charPos");
		glEnableVertexAttribArray(char_attrib_location);
		//std::cout << "char_attrib_location = " << char_attrib_location << std::endl;
		glVertexAttribPointer(char_attrib_location, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}

	void Renderer::render_text(std::string text, float x, float y, float scale, glm::vec3 color)
	{
		// activate corresponding render state	
		glUseProgram(_shader_program_id);
		glBindVertexArray(_vao);
		glActiveTexture(GL_TEXTURE0);
		glUniform3f(glGetUniformLocation(_shader_program_id, "textColor"), color.x, color.y, color.z);
		glUniform1i(glGetUniformLocation(_shader_program_id, "textRender"), 1);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = _characters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;

			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};

			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, _text_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(glGetUniformLocation(_shader_program_id, "textRender"), 0);
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
									//TODO: Make this member check safe
									std::string item_type = image_data_ptr->MemberBegin()->name.GetString();

									full_image_data_path += ("/" + item_type + "/default");
									image_data_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));
									if (image_data_ptr && !image_data_ptr->IsNull())
									{
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
									else
									{
										continue;
									}
								}
								else
								{
									continue;
								}
							}
						}
						else
						{
							continue;
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
						_ref_sets_info.push(std::move(ref_set));
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
			for (auto itr = set.tiles.begin(); itr != (set.tiles.begin() + MAX_COLUMNS_RENDERED);)
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
		while (!_ref_sets_info.empty())
		{
			load_reference_set(_ref_sets_info.front());
			_ref_sets_info.pop();
		}
	}

	bool Renderer::consume_ref_set()
	{
		if (!_ref_sets_info.empty())
		{
			load_reference_set(_ref_sets_info.front());
			_ref_sets_info.pop();
			return true;
		}
		return false;
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

		//TODO: Make this member check safe.
		const auto data_obj_ptr = rapidjson::GetValueByPointer(*ref_set_json_ptr, rapidjson::Pointer("/data"));
		
		if (!data_obj_ptr)
			return;

		//Try to extract a set name. Assumes it's at this location.
		//TODO: Make this safer.
		std::string set_type = data_obj_ptr->MemberBegin()->name.GetString();
		std::string items_arr_path = "/data/" + set_type + "/items";

		const auto items_arr_ptr = rapidjson::GetValueByPointer(*ref_set_json_ptr, rapidjson::Pointer(items_arr_path.c_str()));
		if (items_arr_ptr && !items_arr_ptr->IsNull())
		{
			
			if (items_arr_ptr->IsArray())
			{
				const auto& items_arr = items_arr_ptr->GetArray();
				for (size_t j = 0; j < items_arr.Size(); ++j)
				{
					std::string full_image_data_path = items_arr_path + "/"+ std::to_string(j) +
						"/image/tile/1.78";//TODO: Determine what the integer member of the "tile" object represents.

					auto image_data_ptr = rapidjson::GetValueByPointer(*ref_set_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));

					if (image_data_ptr && !image_data_ptr->IsNull())
					{
						//TODO: Make this member check safe
						std::string item_type = image_data_ptr->MemberBegin()->name.GetString();

						full_image_data_path += ("/" + item_type + "/default");
						image_data_ptr = rapidjson::GetValueByPointer(*ref_set_json_ptr, rapidjson::Pointer(full_image_data_path.c_str()));
						if (image_data_ptr && !image_data_ptr->IsNull())
						{
							if (image_data_ptr->IsObject())
							{
								//TODO: Determine what happens if these members don't exist
								const auto image_url_ptr = image_data_ptr->FindMember("url");
								const auto image_width_ptr = image_data_ptr->FindMember("masterWidth");
								const auto image_height_ptr = image_data_ptr->FindMember("masterHeight");

								if (!image_url_ptr->value.IsString() || !image_width_ptr->value.IsInt() || !image_height_ptr->value.IsInt())
									continue;

								////TODO: Download texture for tile right away?
								//auto texture_ptr = download_texture(image_url_ptr->value.GetString());
								//
								//if (!texture_ptr)
								//	continue;

								//dss_set.tiles.push_back({ image_url_ptr->value.GetString(),
								//	image_width_ptr->value.GetInt(),
								//	image_height_ptr->value.GetInt(),
								//	std::move(texture_ptr) });

								dss_set.tiles.push_back({ image_url_ptr->value.GetString(),
								image_width_ptr->value.GetInt(),
								image_height_ptr->value.GetInt(),
								nullptr });
							}
						}
						else
						{
							return;
						}
					}
					else
					{
						return;
					}
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}

		_sets.push_back(std::move(dss_set));
	}

	void Renderer::draw_home_page()
	{

		//RENDER TEXT FIRST
		float title_y_shift = 0;
		float init_y = 1045.0f;
		for (const auto& set : _sets)
		{
			render_text(set.name, 45, init_y - title_y_shift, 0.4f, glm::vec3(1.0, 1.0f, 1.0f));
			title_y_shift += 270;
		}

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

		//while (rendered_row_count < MAX_SETS_RENDERED && row_index < _sets.size())//LOOP OVER MAX ROW COUNT
		for(int row_index = 0; row_index < MAX_ROWS_RENDERED; ++row_index)//LOOP OVER MAX ROW COUNT
		{
			row_pos = row_index;

			//Check tile frame for any invalid indices
			bool invalid_tile_index_detected = false;
			for (int column_index = 0; column_index < MAX_COLUMNS_RENDERED; ++column_index)
			{
				int tile_index = _tile_indices[row_index][column_index];
				if (tile_index < 0 || tile_index > _sets[row_index].tiles.size())//INVALID FRAME DETECTED
				{
					invalid_tile_index_detected = true;
					break;
				}
			}

			if (invalid_tile_index_detected)
				continue;//Skip rendering for this row


			for (int column_index = 0; column_index < MAX_COLUMNS_RENDERED; ++column_index)//LOOP OVER MAX COLUMN COUNT
			{
				if (column_index >= _sets[row_index].tiles.size())
					break;

				int tile_index = _tile_indices[row_index][column_index];

				if (tile_index == _sets[row_index].tiles.size())//Don't try to render past max tile count
					continue;

				auto& current_tile = _sets[row_index].tiles[tile_index];
				current_tile.position = { row_index, column_index };

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
				glVertexAttribPointer(_position_attrib_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, _tile_tex_vbo);
				glVertexAttribPointer(_texture_coord_attrib_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);


				//Download additional textures as needed.
				if (!current_tile.texture)
				{
					current_tile.texture = download_texture(current_tile.image_url.c_str());
					if (!current_tile.texture)
					{
						continue;
					}
					std::cout << "downloaded additional texture @ " << current_tile.image_url.c_str() << std::endl;
				}

				current_tile.texture->bind(0);
				glDrawArrays(GL_QUADS, 0, 4);
				current_tile.texture->unbind();

				//update counters
				spacing_update_x += 0.37f;
			}

			spacing_update_x = 0;
			spacing_update_y -= 0.5f;
			//++rendered_row_count;
			//++row_index;
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
				if (_focused_tile_position.x == ROWS_UPPER_BOUNDARY_X)
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
				if (_focused_tile_position.x == ROWS_LOWER_BOUNDARY_X)
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
				if (_focused_tile_position.y == COLUMNS_LEFT_BOUNDARY_Y)
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
				if (_focused_tile_position.y == COLUMNS_RIGHT_BOUNDARY_Y)
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

		//std::cout << "focused_tile_position (" << _focused_tile_position.x << " , " << _focused_tile_position.y << " ) " << std::endl;
	}

	void Renderer::check_for_horizontal_boundary_hit(const glm::vec2& pos)
	{
		if (pos.y == COLUMNS_LEFT_BOUNDARY_Y)
		{
			//std::cout << "Horizontal Boundary Hit Detected!" << std::endl;
			//shift tiles right
			if (_tile_indices[(int)pos.x][0] == COLUMNS_LEFT_BOUNDARY_Y)//DON'T UPDATE FRAME!!!
				return;

			for (int column_index = 0; column_index < MAX_COLUMNS_RENDERED; ++column_index)
			{
				int tile_index = _tile_indices[(int)pos.x][column_index] - 1;
				if (tile_index < 0)//new index out of range of current row//DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
					_tile_indices[(int)pos.x][column_index] = tile_index;
				}
			}
		}
		else if (pos.y == COLUMNS_RIGHT_BOUNDARY_Y)
		{
			//std::cout << "Horizontal Boundary Hit Detected!" << std::endl;

			auto current_tile_count = _sets[(int)pos.x].tiles.size();

			//shift tiles left

			//If last index in set of generated tile indices is equal to the number of tiles, do not update indices collection.
			if (_tile_indices[(int)pos.x][MAX_COLUMNS_RENDERED - 1] == (current_tile_count))// - 1))//DON'T UPDATE FRAME!!!
				return;

			for (int column_index = 0; column_index < MAX_COLUMNS_RENDERED; ++column_index)
			{
				int tile_index = _tile_indices[(int)pos.x][column_index] + 1;
				if (tile_index > current_tile_count)//new index out of range of current row //DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
				  _tile_indices[(int)pos.x][column_index] = tile_index;
				}
			}
		}
	}

	void Renderer::check_for_vertical_boundary_hit(const glm::vec2& pos)
	{
		if (pos.x == ROWS_UPPER_BOUNDARY_X)
		{
			//std::cout << "Vertical Boundary Hit Detected!" << std::endl;

			//shift tiles down
			if (_set_indices[0] == ROWS_UPPER_BOUNDARY_X)//DON'T UPDATE FRAME!!!
				return;

			for (int row_index = 0; row_index < MAX_ROWS_RENDERED; ++row_index)
			{
				int set_index = _set_indices[row_index] - 1;
				if (set_index < 0)//new index out of range of current row//DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
					_set_indices[row_index] = set_index;
				}
			}
		}
		else if (pos.x == ROWS_LOWER_BOUNDARY_X)
		{
			//std::cout << "Vertical Boundary Hit Detected!" << std::endl;
			//shift tiles up

			//if (!consume_ref_set())//TODO: BUG!!!! THIS SHOULD NOT RETURN IF NO REF SET TO CONSUME
				//return;

			consume_ref_set();

			auto current_set_count = _sets.size();//NOTE!! This collection's size will increase dynamically during runtime. Keep that in mind!!!

			//If last index in set of generated set indices is equal to last index available for sets, do not update indices collection.
			if (_set_indices[MAX_ROWS_RENDERED - 1] == ((current_set_count) - 1))//DON'T UPDATE FRAME!!!
				return;

			for (int row_index = 0; row_index < MAX_ROWS_RENDERED; ++row_index)
			{
				int set_index = _set_indices[row_index] + 1;
				if (set_index >= current_set_count)//new index out of range of current row //DON'T ADD INVALID INDEX
				{
					return;
				}
				else
				{
					_set_indices[row_index] = set_index;
				}
			}
		}
	}
}