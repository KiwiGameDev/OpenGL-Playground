#pragma once

#include "image.h"

enum class TextureType
{
	Diffuse = 0,
	Normal,
	Specular
};

struct TextureAsset
{
	std::string FilePath;
	TextureType Type;

	TextureAsset(std::string filePath, TextureType type)
		: FilePath(filePath), Type(type) { }
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

	Texture(TextureAsset textureAsset)
		: textureType(textureAsset.Type)
	{
		Image img(textureAsset.FilePath);

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
