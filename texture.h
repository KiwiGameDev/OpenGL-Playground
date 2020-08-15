#pragma once
#include "image.h"

#include <algorithm>

enum class TextureType
{
	Diffuse = 0,
	Normal,
	Specular
};

class Texture
{
public:
	static const char* ShaderUniforms[];

	unsigned int ID;
	TextureType textureType;

	Texture()
	{
		ID = 4096;
		textureType = TextureType();
	}

	Texture(std::string filePath)
	{
		Image img(filePath);

		std::transform(filePath.begin(), filePath.end(), filePath.begin(), std::tolower);
		if (filePath.find("normal") != std::string::npos)
			textureType = TextureType::Normal;
		else if (filePath.find("specular") != std::string::npos)
			textureType = TextureType::Specular;
		else
			textureType = TextureType::Diffuse;

		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			img.width,
			img.height,
			0,
			img.glChannels,
			GL_UNSIGNED_BYTE,
			img.data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void bindTexture(unsigned int shaderID, int samplerID) const
	{
		std::string textureName(ShaderUniforms[(int)textureType]);
		GLint textureLoc = glGetUniformLocation(shaderID, textureName.c_str());
		glUniform1i(textureLoc, samplerID);
		glActiveTexture(GL_TEXTURE0 + samplerID);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
};

const char* Texture::ShaderUniforms[] =
{
	"u_material.diffuseMap",
	"u_material.normalMap",
	"u_material.specularMap"
};
