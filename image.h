#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>

struct Image
{
	unsigned char* data;
	int width;
	int height;
	int channelCount;
	int format;

	Image(const std::string& filePath, bool flipVertically = true)
	{
		stbi_set_flip_vertically_on_load(flipVertically);
		data = stbi_load(filePath.c_str(), &width, &height, &channelCount, 0);
		
		if (channelCount == 1)
			format = GL_RED;
		else if (channelCount == 2)
			format = GL_RG;
		else if (channelCount == 3)
			format = GL_RGB;
		else if (channelCount == 4)
			format = GL_RGBA;
	}

	~Image()
	{
		stbi_image_free(data);
	}
};