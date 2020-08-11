#pragma once

struct Image
{
	unsigned char* data;
	int width;
	int height;
	int channelCount;
	int glChannels;

	Image(const std::string& filepath)
	{
		stbi_set_flip_vertically_on_load(true);
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

class Texture
{
public:
	unsigned int ID;

	Texture(const std::string& filepath)
	{
		Image img(filepath);
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, img.glChannels, GL_UNSIGNED_BYTE, img.data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void bindTexture(unsigned int shaderID, int samplerID)
	{
		std::string textureName = getTextureName(samplerID);
		GLint textureLoc = glGetUniformLocation(shaderID, textureName.c_str());
		glUniform1i(textureLoc, samplerID);
		glActiveTexture(GL_TEXTURE0 + samplerID);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

private:
	std::string getTextureName(int samplerID)
	{
		return "u_material." + textureNames[samplerID];
	}

	const std::string textureNames[3] =
	{
		"diffuseMap",
		"normalMap",
		"specularMap"
	};
};
