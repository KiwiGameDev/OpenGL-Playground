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

	Particle(const VertexArrayObject& vao, const Shader& shader)
		: Drawable(vao, shader)
	{
		Velocity = glm::vec3(randf() - 0.5f, randf() - 0.5f, randf() - 0.5f);
		RotationSpeed = glm::vec3(randf() * 0.5f - 0.25f, randf() * 0.5f - 0.25f, randf() * 0.5f - 0.25f);
		Scale = glm::vec3(0.1f, 0.1f, 0.1f);
	}

	void update(float deltaTime)
	{
		Position += Velocity * deltaTime;
		Rotation += RotationSpeed * deltaTime;
		updateModelMatrix();
	}

	void updateShaderUniforms(const glm::mat4& viewProjection) const
	{
		shader.setMat4("u_model", Model);
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}
};