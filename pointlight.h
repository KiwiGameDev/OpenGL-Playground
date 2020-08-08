#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vector3.h"
#include "vertexarrayobject.h"

class PointLight
{
public:
	VertexArrayObject VAO;

	Vector3f Ambient;
	Vector3f Diffuse;
	Vector3f Specular;

	float Constant;
	float Linear;
	float Quadratic;

	glm::mat4 Model;
	Vector3f Position;
	Vector3f Rotation;
	Vector3f Scale;

	PointLight(VertexArrayObject vao)
		: VAO(vao)
	{
		Ambient = { 0.2f, 0.2f, 0.2f };
		Diffuse = { 0.8f, 0.8f, 0.8f };
		Specular = { 1.0f, 1.0f, 1.0f };

		Constant = 1.0f;
		Linear = 0.09f;
		Quadratic = 0.032f;

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

	void draw()
	{
		VAO.draw();
	}
};