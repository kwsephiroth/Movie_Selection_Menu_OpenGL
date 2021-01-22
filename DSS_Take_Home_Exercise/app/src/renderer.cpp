#include "renderer.h"
#include <iostream>//temp

namespace DSS
{
	Renderer::Renderer()
	{
		init();
	}

	void Renderer::init()
	{
		//TODO: Determine if this function should throw an exception if it fails to initialize the object state.
		load_homepage_api_json();
		if (_home_json_ptr)
		{
			load_textures();
		}
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
		//std::cout << "total item count = " << total_items << std::endl;//temp
		//std::cout << "total image count = " << total_images << std::endl;//temp
		//std::cout << "SETS" << std::endl;
		//for (const auto& set : _sets)
		//{
		//	std::cout << set.name << " : " << set.tiles.size() << " tiles" << std::endl;
		//}
		//std::cout << std::endl;
		//std::cout << "REF SET INFO" << std::endl;
		//for (const auto& ref_set_info : _ref_sets_info)
		//{
		//	std::cout << "name: " << ref_set_info.name << "\nurl: " << ref_set_info.ref_set_url << "\n\n";
		//}
	}

	void Renderer::load_textures()
	{
		//int j = 1;
		for (auto& set : _sets)
		{	
			for (auto& tile : set.tiles)
			{
				//TODO: Use image url and curl to load tile images into memory.
				//std::string out_file_name = "image" + std::to_string(j) + ".png";
				//std::ofstream out_image(out_file_name, std::ios::binary);
				/*if (!out_image.good())
				{
					std::cout << "Failed to open image output file." << std::endl;
					return;
				}*/
				//std::cout << "downloading image from " << tile.image_url << std::endl;
				auto file_memory_ptr = curl_utils::download_file_to_memory(tile.image_url.c_str());
				if (file_memory_ptr)
				{
					int width = tile.image_width;
					int height = tile.image_height;
					int channels = 0;
					std::unique_ptr<unsigned char> image_buffer_ptr(SOIL_load_image_from_memory((const unsigned char*)file_memory_ptr->memory,
						file_memory_ptr->size,
						&width,
						&height,
						&channels,
						SOIL_LOAD_AUTO));
					//SOIL_save_image(out_file_name.c_str(), SOIL_SAVE_TYPE_PNG, width, height, channels, image_buffer);
					tile.texture.reset( new Texture(std::move(image_buffer_ptr), width, height ));
				}
				//out_image.close();
				//++j;
				
			}
		}
	}
}