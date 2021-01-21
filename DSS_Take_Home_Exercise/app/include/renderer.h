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

namespace DSS
{
	class Renderer//TODO: Determine if this will need to be a singleton class.
	{
	private:
		std::unique_ptr<rapidjson::Document> _home_json_ptr;//TODO: Consider having this dependency injected through constructor.
		
		void init();
		void load_homepage_api_json();

	public:
		Renderer();
	};
}
#endif
