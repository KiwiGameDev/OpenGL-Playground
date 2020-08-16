#pragma once

#include "vertexarrayobject.h"
#include "shader.h"

class Drawable
{
public:
	VertexArrayObject VAO;
	Shader shader;

	Drawable()
		: VAO(), shader() { }

	Drawable(const VertexArrayObject& vao, const Shader& shader)
		: VAO(vao), shader(shader) { }

	virtual void bind() const
	{
		VAO.bind();
		shader.use();
	}

	virtual void updateShaderUniforms(const glm::mat4& viewProjection) const = 0;
	
	void draw() const
	{
		VAO.draw();
	}
};