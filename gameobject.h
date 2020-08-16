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

	GameObject()
	{
		Velocity = glm::vec3();
		RotationSpeed = glm::vec3();
	}

	GameObject(const std::vector<VertexArrayObject>& vaos, const std::vector<Material>& materials)
		: Drawable(vaos, materials)
	{
		Velocity = glm::vec3();
		RotationSpeed = glm::vec3();
	}

	GameObject(const VertexArrayObject& vao, const Material& material)
		: Drawable(vao, material)
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

	void updateShaderUniforms(const Shader& shader, const glm::mat4& viewProjection) const
	{
		shader.setMat4("u_model", Model);
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}

	void updateShaderUniforms(const Shader& shader) const
	{
		shader.setFloat("u_material.shininess", 32.0f);
	}
};
