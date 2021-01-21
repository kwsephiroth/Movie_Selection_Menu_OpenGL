#ifndef HELPERS_H_
#define HELPERS_H_

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <optional>
#include <memory>

namespace Utils
{
	static std::optional<rapidjson::Document> get_json_doc_from_file(const char* filename)
	{
		FILE* fp = fopen(filename, "rb"); // non-Windows use "r"

		if (!fp)
		{
			return std::nullopt;
		}

		std::unique_ptr<char> readBuffer(new char[65536]);
		//char* readBuffer = new char[65536];
		rapidjson::FileReadStream is(fp, readBuffer.get(), sizeof(*readBuffer.get()));
		rapidjson::Document d;
		d.ParseStream(is);

		fclose(fp);

		//readBuffer = nullptr;
		//delete [] readBuffer;

		if (!d.HasParseError())
		{
			return d;
		}

		return std::nullopt;
	}
}
#endif // HELPERS