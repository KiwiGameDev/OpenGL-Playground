#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "transformable.h"
#include "vertexarrayobject.h"

class PointLight : public Transformable
{
public:
	VertexArrayObject VAO;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	float Constant;
	float Linear;
	float Quadratic;

	PointLight(VertexArrayObject vao)
		: VAO(vao)
	{
		Ambient = { 0.2f, 0.2f, 0.2f };
		Diffuse = { 0.8f, 0.8f, 0.8f };
		Specular = { 1.0f, 1.0f, 1.0f };

		Constant = 1.0f;
		Linear = 0.09f;
		Quadratic = 0.032f;
	}

	void bind()
	{
		glBindVertexArray(VAO.ID);
	}

	void draw()
	{
		VAO.draw();
	}
};