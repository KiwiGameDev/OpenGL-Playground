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

	Texture(const std::string& filePath)
	{
		Image img(filePath);

		textureType = getTextureType(filePath);

		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			img.format,
			img.width,
			img.height,
			0,
			img.format,
			GL_UNSIGNED_BYTE,
			img.data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void bindTexture(unsigned int shaderID, int textureTypeID) const
	{
		std::string textureName(ShaderUniforms[(int)textureType]);
		GLint textureLoc = glGetUniformLocation(shaderID, textureName.c_str());
		glUniform1i(textureLoc, textureTypeID);
		glActiveTexture(GL_TEXTURE0 + textureTypeID);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

private:
	TextureType getTextureType(std::string path)
	{
		std::transform(path.begin(), path.end(), path.begin(), std::tolower);

		if (path.find("normal") != std::string::npos)
			return TextureType::Normal;
		else if (path.find("specular") != std::string::npos)
			return TextureType::Specular;
		else
			return TextureType::Diffuse;
	}
};

const char* Texture::ShaderUniforms[] =
{
	"u_material.diffuseMap",
	"u_material.normalMap",
	"u_material.specularMap"
};
