#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <unordered_map>

#include "texture.h"
#include "shader.h"
#include "transformable.h"
#include "drawable.h"

class GameObject : public Transformable, public Drawable
{
public:
	glm::vec3 Velocity;
	glm::vec3 RotationSpeed;
	std::unordered_map<TextureType, Texture> textures;

	GameObject()
	{
		Velocity = glm::vec3();
		RotationSpeed = glm::vec3();
	}

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

	void addTexture(Texture texture)
	{
		textures.insert(std::make_pair(texture.textureType, texture));
	}

	void bindTextures() const
	{
		int i = 0;
		for (const auto& itr : textures)
			itr.second.bindTexture(shader.ID, i++);
	}
};
