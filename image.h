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
	int glChannels;

	Image(const std::string& filepath, bool flipVertically = true)
	{
		stbi_set_flip_vertically_on_load(flipVertically);
		data = stbi_load(filepath.c_str(), &width, &height, &channelCount, 0);

		if (channelCount == 3)
			glChannels = GL_RGB;
		else if (channelCount == 4)
			glChannels = GL_RGBA;
		else
			glChannels = 0;
	}

	~Image()
	{
		stbi_image_free(data);
	}
};