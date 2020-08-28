#pragma once
#include "texture.h"
#include "shader.h"

#include <vector>

class Material
{
public:
	float Diffuse[3];
	float Specular[3];
	float Shininess;

	Material()
	{
		Diffuse[0] = -1.0f;
		Diffuse[1] = -1.0f;
		Diffuse[2] = -1.0f;

		Specular[0] = -1.0f;
		Specular[1] = -1.0f;
		Specular[2] = -1.0f;

		Shininess = 32.0f;
	}

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

	void setMaterialUniforms() const
	{
		shader.setFloat("u_material.shininess", Shininess);

		if (Diffuse[0] != -1.0f)
			shader.setVec3("u_material.diffuse", Diffuse);
		if (Specular[0] != -1.0f)
			shader.setVec3("u_material.specular", Specular);
	}

	void bind() const
	{
		if (diffuseTexture.ID != 4096)
		{
			diffuseTexture.bindTexture(shader.ID, 0);
			normalTexture.bindTexture(shader.ID, 1);
			specularTexture.bindTexture(shader.ID, 2);
		}
	}

private:
	Shader shader;
	Texture diffuseTexture;
	Texture normalTexture;
	Texture specularTexture;
};