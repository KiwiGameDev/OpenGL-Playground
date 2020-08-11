#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class Transformable
{
public:
	glm::mat4 Model;
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	Transformable()
	{
		Model = glm::mat4(1.0f);
		Position = glm::vec3();
		Rotation = glm::vec3();
		Scale = glm::vec3(1.0f, 1.0f, 1.0f);
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

	glm::mat4 getMVP(const glm::mat4& worldToClip)
	{
		return worldToClip * Model;
	}
};