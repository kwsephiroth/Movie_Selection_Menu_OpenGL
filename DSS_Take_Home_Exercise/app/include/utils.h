#ifndef UTILS_H_
#define UTILS_H_
#include <rapidjson/document.h>
#include <memory>
#include "curl_utils.h"
namespace DSS
{
	namespace Utils
	{
		static std::unique_ptr<rapidjson::Document> get_rj_document(const char* file_data)
		{
			auto doc_ptr = std::make_unique<rapidjson::Document>();
			doc_ptr->Parse(file_data);
			if (doc_ptr->HasParseError())
			{
				return nullptr;
			}
			return doc_ptr;
		}
	}
}

#endif