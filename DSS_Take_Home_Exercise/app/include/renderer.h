#ifndef RENDERER_H_
#define RENDERER_H_
#include "utils.h"
#include <rapidjson/document.h>
#include <memory>
#include "curl_utils.h"
#include "constants.h"
#include "rapidjson/pointer.h"
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

namespace DSS
{
	struct Set
	{
		std::string name;
		std::vector<std::string> tiles;//TODO: Create type to store tile metadata
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
		void init();
		void load_homepage_api_json();

	public:
		Renderer();
	};
}
#endif
