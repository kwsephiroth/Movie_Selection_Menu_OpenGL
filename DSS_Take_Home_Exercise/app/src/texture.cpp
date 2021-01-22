#include "texture.h"
#include <iostream>//temp

namespace DSS
{
	Texture::Texture(std::unique_ptr<unsigned char> img_data_ptr, int width, int height) :
		_img_data_ptr(std::move(img_data_ptr)), _width(width), _height(height)
	{
		//static int i = 1;
		//std::cout << "Texture (" << i << ") created!" << std::endl;
		//++i;
	}
}