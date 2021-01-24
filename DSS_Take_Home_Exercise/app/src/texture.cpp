#include "texture.h"
#include <iostream>//temp

namespace DSS
{
	Texture::Texture(std::unique_ptr<unsigned char> img_data_ptr, int width, int height) :
		_img_data_ptr(std::move(img_data_ptr)), _width(width), _height(height)
	{
		if (_img_data_ptr)
		{
			load_texture();
			_initialized = true;
		}

		//TODO: What to do if image data is null?
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_index);
	}

	void Texture::load_texture()
	{
		glGenTextures(1, &_index);
		glBindTexture(GL_TEXTURE_2D, _index);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _img_data_ptr.get());
		glGenerateMipmap(GL_TEXTURE_2D);

		//free image data after loading
		//SOIL_free_image_data(_img_data_ptr.get());
		//_img_data_ptr.reset(nullptr);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::bind(unsigned int slot)
	{
		assert(slot >= 0 && slot <= 31);

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _index);
	}

	void Texture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}