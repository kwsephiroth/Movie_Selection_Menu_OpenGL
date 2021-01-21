#include "renderer.h"

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
	}

	void Renderer::load_homepage_api_json()
	{
		//Attempt to download api json using url
		{
			auto file_memory_ptr = curl_utils::download_file_to_memory(DSS_HOME_JSON_URL);
			_home_json_ptr = Utils::get_rj_document(file_memory_ptr->memory);
			assert(_home_json_ptr);
		}
static int total_items = 0;//temp
static int total_images = 0;//temp
		//Get "containers" array //TODO: Might need to store these array pointers as class members for faster lookup or make them static
		const auto containers_arr_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer("/data/StandardCollection/containers"));
		if (containers_arr_ptr && !containers_arr_ptr->IsNull())
		{
			if (containers_arr_ptr->IsArray())
			{
				const auto& containers_arr = containers_arr_ptr->GetArray();
				for (size_t i = 0; i < containers_arr.Size(); ++i)
				{
					std::string items_arr_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/items";
					const auto items_arr_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(items_arr_path.c_str()));
					if (items_arr_ptr && !items_arr_ptr->IsNull())
					{
						if (items_arr_ptr->IsArray())
						{
							total_items += items_arr_ptr->GetArray().Size();
							const auto& items_arr = items_arr_ptr->GetArray();
							for (size_t j = 0; j < items_arr.Size(); ++j)
							{
								std::string full_item_name_path = "/data/StandardCollection/containers/" + std::to_string(i) + "/set/items/" + std::to_string(j) +
									"/image/tile/1.78";// / series / default / url";// / text / title / full / series / default / content";
								
								auto full_item_name_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_item_name_path.c_str()));

								if (full_item_name_ptr && !full_item_name_ptr->IsNull())
								{
									std::string item_type = full_item_name_ptr->MemberBegin()->name.GetString();
									full_item_name_path += "/" + item_type + "/default/url";
									full_item_name_ptr = rapidjson::GetValueByPointer(*_home_json_ptr, rapidjson::Pointer(full_item_name_path.c_str()));
									if (full_item_name_ptr && !full_item_name_ptr->IsNull())
									{
										if (full_item_name_ptr->IsString())
										{
											total_images++;
											std::cout << "image url = " << full_item_name_ptr->GetString() << std::endl;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		std::cout << "total item count = " << total_items << std::endl;
		std::cout << "total image count = " << total_images << std::endl;
	}
}