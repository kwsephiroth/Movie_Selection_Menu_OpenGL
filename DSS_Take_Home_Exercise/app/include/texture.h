#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <GL/glew.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <vector>

namespace DSS
{
	class Texture
	{
	private:
		int _width;
		int _height;
		GLuint _index;
		std::string _image_file_path;
		unsigned char* _image = nullptr; //Raw pointer!!! Either eliminate, write custom copy constructor and assignment operator, or disable copying

		void load_texture();

	public:
		Texture(const std::string& image_file_path);
		Texture(char* image_data, unsigned int width, unsigned int height);
		GLuint get_index() const { return _index; }
		int get_height() const { return _height; }
		int get_width() const { return _width; }
		const std::string& get_image_file_path() const { return _image_file_path; }
	};
}

#endif