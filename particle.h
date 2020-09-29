#pragma once
#include <glm/glm.hpp>

#include "helpers.h"
#include "vertexarrayobject.h"
#include "shader.h"
#include "transformable.h"
#include "drawable.h"

class Particle : public Transformable, public Drawable
{
public:
	glm::vec3 Velocity;
	glm::vec3 RotationSpeed;

	Particle(const VertexArrayObject& vao, const Material& material)
		: Drawable(std::vector<VertexArrayObject>() = { vao }, std::vector<Material>() = { material })
	{
		Position = glm::vec3(randf() * 8.0f + 2.0f, randf() * 8.0f - 4.0f, -randf() * 12.0f);
		Velocity = glm::vec3(-1.0f + -randf() * 2.0f, randf() * -1.0f, randf() * 2.0f);
		RotationSpeed = glm::vec3(randf() * 0.5f - 0.25f, randf() * 0.5f - 0.25f, randf() * 0.5f - 0.25f);
		Scale = glm::vec3(0.1f, 0.1f, 0.1f);
		lifetime = randf() * 5.0f + 5;
		timer = 0;
	}

	void update(float deltaTime)
	{
		timer += deltaTime;
		if (timer >= lifetime)
		{
			Position = glm::vec3(randf() * 8.0f + 2.0f, randf() * 8.0f - 4.0f, -randf() * 12.0f);
			Velocity = glm::vec3(-1.0f + -randf() * 2.0f, randf() * -1.0f, randf() * 2.0f);
			lifetime = randf() * 5.0f + 5.0f;
			timer = 0;
		}

		Position += Velocity * deltaTime;
		Rotation += RotationSpeed * deltaTime;
		updateModelMatrix();
	}

	void updateShaderUniforms(const Shader& shader, const glm::mat4& viewProjection) const
	{
		shader.setMat4("u_model", Model);
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}

	void updateShaderUniforms(const Shader& shader) const
	{

	}

private:
	float timer;
	float lifetime;
};