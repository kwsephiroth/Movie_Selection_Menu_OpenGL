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
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &_index);
	}

	void Texture::bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
	    glBindTexture(GL_TEXTURE_2D, _index);
	}

	void Texture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::load_texture()
	{
		glGenTextures(1, &_index);
		glBindTexture(GL_TEXTURE_2D, _index);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _img_data_ptr.get());
		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _img_data_ptr.get());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture when done, so we won't accidentily mess up our texture.
		
		//TODO: Determine if it's safe to free image data here.
	}
}