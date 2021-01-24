#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <GL/glew.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

namespace DSS
{
	class Texture
	{
	protected:
		int _width;
		int _height;
		GLuint _index;
		std::string _image_file_path;
		std::unique_ptr<unsigned char> _img_data_ptr;
		bool _initialized;

		void load_texture();

	public:
		Texture(std::unique_ptr<unsigned char> img_data_ptr, int width, int height);
		virtual ~Texture();

		//No copying
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		inline GLuint get_index() const { return _index; }
		inline int get_height() const { return _height; }
		inline int get_width() const { return _width; }
		inline bool is_initialized() const { return _initialized; }
		const std::string& get_image_file_path() const { return _image_file_path; }
	
		void bind(unsigned int slot);
		void unbind();
	};
}

#endif