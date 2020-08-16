#pragma once
#include "vertexarrayobject.h"
#include "material.h"

class Drawable
{
public:
	std::vector<VertexArrayObject> VAOs;
	std::vector<Material> Materials;

	Drawable() { }

	Drawable(const VertexArrayObject& vaos, const Material& material)
		: VAOs(std::vector<VertexArrayObject>() = { vaos }), Materials(std::vector<Material>() = { material }) { }

	Drawable(const std::vector<VertexArrayObject>& vaos, const std::vector<Material>& materials)
		: VAOs(vaos), Materials(materials) { }
	
	void draw(const glm::mat4& viewProjection) const
	{
		for (int i = 0; i < VAOs.size(); i++)
		{
			const VertexArrayObject& vao = VAOs[i];
			const Material& material = Materials[i];

			vao.bind();
			material.useShader();
			updateShaderUniforms(material.getShader(), viewProjection);
			updateShaderUniforms(material.getShader());
			material.bind();
			vao.draw();
		}
	}

protected:
	virtual void updateShaderUniforms(const Shader& shader, const glm::mat4& viewProjection) const = 0;
	virtual void updateShaderUniforms(const Shader& shader) const = 0;
};