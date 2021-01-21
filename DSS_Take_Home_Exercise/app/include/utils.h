#ifndef UTILS_H_
#define UTILS_H_
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

//static std::optional<rapidjson::Document> get_json_doc_from_file(const char* filename)
//{
//	FILE* fp = fopen(filename, "rb"); // non-Windows use "r"

//	if (!fp)
//	{
//		return std::nullopt;
//	}

//	std::unique_ptr<char> readBuffer(new char[65536]);
//	//char* readBuffer = new char[65536];
//	rapidjson::FileReadStream is(fp, readBuffer.get(), sizeof(*readBuffer));//TODO: Consume home.json file stream here.
//	rapidjson::Document d;
//	d.ParseStream(is);

//	fclose(fp);

//	//readBuffer = nullptr;
//	//delete [] readBuffer;

//	if (!d.HasParseError())
//	{
//		return d;
//	}

//	return std::nullopt;
//}

#endif // UTILS