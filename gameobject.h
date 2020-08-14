#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "shader.h"
#include "AssetManager.h"
#include "transformable.h"
#include "drawable.h"

class GameObject : public Transformable, public Drawable
{
public:
	glm::vec3 Velocity;
	glm::vec3 RotationSpeed;
	std::vector<Texture> textures;

	GameObject(VertexArrayObject vao, Shader shader)
		: Drawable(vao, shader)
	{
		Velocity = glm::vec3();
		RotationSpeed = glm::vec3();
	}

	void update(float deltaTime)
	{
		Position += Velocity * deltaTime;
		Rotation += RotationSpeed * deltaTime;
		updateModelMatrix();
	}

	void updateShaderUniforms(glm::mat4 viewProjection) const
	{
		shader.setMat4("u_model", Model);
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}

	void bindTextures() const
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].bindTexture(shader.ID, i);
	}
};
