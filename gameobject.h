#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "Vector3.h"

struct GameObject
{
	GLuint vaoID;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector3f> vertexColors;
	std::vector<Vector3u> indices;

	glm::mat4 model;
	Vector3f position;
	Vector3f velocity;
	Vector3f rotation;

	GameObject()
	{
		vaoID = -1;

		vertexPositions = std::vector<Vector3f>();
		vertexNormals = std::vector<Vector3f>();
		vertexColors = std::vector<Vector3f>();
		indices = std::vector<Vector3u>();

		model = glm::mat4(1.0f);
		position = Vector3f();
		velocity = Vector3f();
		rotation = Vector3f();
	}
};