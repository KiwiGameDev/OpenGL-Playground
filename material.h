#pragma once
#include "texture.h"
#include "shader.h"

#include <vector>

class Material
{
public:
	Material() { }

	Material(const std::vector<Texture>& textures, const Shader& shader)
		: shader(shader)
	{
		for (const Texture& texture : textures)
			setTexture(texture);
	}

	void useShader() const
	{
		shader.use();
	}

	void setTexture(const Texture& texture)
	{
		if (texture.textureType == TextureType::Diffuse)
			diffuseTexture = texture;
		else if (texture.textureType == TextureType::Normal)
			normalTexture = texture;
		else if (texture.textureType == TextureType::Specular)
			specularTexture = texture;
	}

	void setShader(const Shader& shader)
	{
		this->shader = shader;
	}

	const Shader& getShader() const
	{
		return shader;
	}

	void bind() const
	{
		diffuseTexture.bindTexture(shader.ID, 0);
		normalTexture.bindTexture(shader.ID, 1);
		specularTexture.bindTexture(shader.ID, 2);
	}

private:
	Shader shader;
	Texture diffuseTexture;
	Texture normalTexture;
	Texture specularTexture;
};