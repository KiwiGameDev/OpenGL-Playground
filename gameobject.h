#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "vector2.h"
#include "vector3.h"
#include "helpers.h"
#include "AssetManager.h"

struct GameObject
{
	VertexArrayObject VAO;

	std::vector<Texture> textures;

	glm::mat4 Model;
	Vector3f Position;
	Vector3f Rotation;
	Vector3f Scale;

	GameObject(VertexArrayObject VAO)
		: VAO(VAO)
	{
		textures = std::vector<Texture>();

		Model = glm::mat4(1.0f);
		Position = Vector3f();
		Rotation = Vector3f();
		Scale = Vector3f(1.0f, 1.0f, 1.0f);
	}

	void bind()
	{
		glBindVertexArray(VAO.ID);
	}

	void updateModelMatrix()
	{
		Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(Position.x, Position.y, Position.z));
		Model = glm::rotate(Model, Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		Model = glm::rotate(Model, Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		Model = glm::rotate(Model, Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::scale(Model, glm::vec3(Scale.x, Scale.y, Scale.z));
	}

	glm::mat4 getLocalToClipMatrix(const glm::mat4& worldToClip)
	{
		return worldToClip * Model;
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
