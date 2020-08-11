#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "helpers.h"
#include "AssetManager.h"
#include "transformable.h"

class GameObject : public Transformable
{
public:
	VertexArrayObject VAO;

	std::vector<Texture> textures;

	GameObject(VertexArrayObject VAO)
		: VAO(VAO)
	{
		textures = std::vector<Texture>();
	}

	void bind()
	{
		glBindVertexArray(VAO.ID);
	}

	void bindTextures(unsigned int shaderID)
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].bindTexture(shaderID, i);
	}

	void draw()
	{
		VAO.draw();
	}
};
