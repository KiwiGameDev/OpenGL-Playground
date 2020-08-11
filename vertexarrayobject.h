#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>

class VertexArrayObject
{
public:
	unsigned int ID;
	unsigned int VertexPositionID;
	unsigned int VertexTexCoordsID;
	unsigned int VertexNormalsID;
	unsigned int VertexTangentsID;
	unsigned int IndicesID;
	size_t IndicesSize;

	void bind() const
	{
		glBindVertexArray(ID);
	}

	void draw() const
	{
		glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, 0);
	}
};