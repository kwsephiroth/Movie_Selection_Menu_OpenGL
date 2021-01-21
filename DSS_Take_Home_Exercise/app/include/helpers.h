#ifndef HELPERS_H_
#define HELPERS_H_

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <optional>

namespace DSS
{
	std::optional<rapidjson::Document> get_json_doc_from_file(const char* filename)
	{
		FILE* fp = fopen(filename, "rb"); // non-Windows use "r"

		if (!fp)
		{
			return std::nullopt;
		}

		char readBuffer[65536];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

		rapidjson::Document d;
		d.ParseStream(is);

		fclose(fp);

		if (!d.HasParseError())
		{
			return d;
		}

		return std::nullopt;
	}
}
#endif // HELPERS