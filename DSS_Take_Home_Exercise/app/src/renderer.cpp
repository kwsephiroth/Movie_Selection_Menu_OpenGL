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
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::init()
	{
		std::cout << "Initializing homepage ...." << std::endl;
		//TODO: Determine if this function should throw an exception if it fails to initialize the object state.
		load_homepage_api_json();
		if (_home_json_ptr)
		{
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

	void Renderer::init_meshes()
	{
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
		//int j = 1;
		for (auto& set : _sets)
		{	
			for (auto& tile : set.tiles)
			{
				//TODO: Use image url and curl to load tile images into memory.
				//std::string out_file_name = "image" + std::to_string(j) + ".jpeg";
				//std::ofstream out_image(out_file_name, std::ios::binary);
				/*if (!out_image.good())
				{
					std::cout << "Failed to open image output file." << std::endl;
					return;
				}*/
				//std::cout << "downloading image from " << tile.image_url << std::endl;
				//std::cout << "Loading texture for file @ " << tile.image_url << std::endl;
				auto file_memory_ptr = curl_utils::download_file_to_memory(tile.image_url.c_str());
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
						//std::cout << "image was null for url " << tile.image_url << " and size = " << file_memory_ptr->size << std::endl;
						continue;
					}

					//SOIL_save_image(out_file_name.c_str(), SOIL_SAVE_TYPE_JPG, width, height, channels, image_buffer_ptr.get());
					tile.texture.reset( new Texture(std::move(image_buffer_ptr), width, height ));
					//std::cout << "width = " << width << " , height = " << height << " , tile.master_width = " << tile.master_width << " , tile.master_height = " << tile.master_height
						//<< " , channels = " << channels << std::endl;
				}
				//out_image.close();
				//if (j == 10) return;
				//++j;
				//return;//temp - just download one file
				
			}
		}
	}

	void Renderer::draw_home_page()
	{
		glUseProgram(_shader_program_id);

		static const float SIZE_OFFSET = 0.3;
		static const float POS_OFFSET_X = -2.80;
		static const float POS_OFFSET_Y = 2.3;

		glBindVertexArray(_vao);
		
		float pos_update_x = 0;
		float pos_update_y = 0;
		size_t rendered_tile_count = 0;
		size_t rendered_row_count = 0;
		size_t row_index = 0;
		size_t column_index = 0;

		while (rendered_row_count < 4 && row_index < _sets.size())//create 4 rows of tiles
		{
			while (rendered_tile_count < 5 && column_index < _sets[row_index].tiles.size())//create 5 columns of tiles
			{
				auto& current_tile = _sets[row_index].tiles[column_index];
				current_tile.position = { pos_update_x, pos_update_y };
				++column_index;
				
				if (!current_tile.texture)//Ignore any tiles that weren't properly initialized
				{
					continue;
				}

				//TODO: Apply any transformations to tiles
				glm::mat4 transform(1);//Initialize to identity matrix
				transform = glm::scale(transform, glm::vec3(SIZE_OFFSET, SIZE_OFFSET, 0.0f));
				transform = glm::translate(transform, glm::vec3(POS_OFFSET_X + pos_update_x, POS_OFFSET_Y + pos_update_y, 0.0f));
				GLuint transLoc = glGetUniformLocation(_shader_program_id, "transform");
				glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
				//

				glBindBuffer(GL_ARRAY_BUFFER, _tile_pos_vbo);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, _tile_tex_vbo);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(1);

				//glEnable(GL_TEXTURE_2D);
				current_tile.texture->bind(0);
				glDrawArrays(GL_QUADS, 0, 4);
				current_tile.texture->unbind();

				pos_update_x += 1.4;
				++rendered_tile_count;
			}

			//reset column counts
			pos_update_x = 0;
			rendered_tile_count = 0;
			column_index = 0;

			pos_update_y -= 1.4;
			++rendered_row_count;
			++row_index;
		}
		glBindVertexArray(0);
	}

	void Renderer::process_controller_input(const ControllerInput input)//TODO: Determine if this callback is thread-safe. this will update a current position member.
	{
		switch (input)
		{
			case ControllerInput::UP:
			{

			}
			break;

			case ControllerInput::DOWN:
			{

			}
			break;

			case ControllerInput::LEFT:
			{

			}
			break;

			case ControllerInput::RIGHT:
			{

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
			}
		}
	}
}