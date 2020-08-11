#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "transformable.h"
#include "drawable.h"
#include "vertexarrayobject.h"

class PointLight : public Transformable, public Drawable
{
public:
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	float Constant;
	float Linear;
	float Quadratic;

	PointLight(VertexArrayObject vao, Shader shader)
		: Drawable(vao, shader)
	{
		Ambient = { 0.2f, 0.2f, 0.2f };
		Diffuse = { 0.8f, 0.8f, 0.8f };
		Specular = { 1.0f, 1.0f, 1.0f };

		Constant = 1.0f;
		Linear = 0.09f;
		Quadratic = 0.032f;
	}

	void updateShaderUniforms(glm::mat4 viewProjection) const
	{
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}

	void setOtherShaderUniforms(Shader shader, int index) const
	{
		std::string strUniform = "u_pointLights[" + std::to_string(index) + "]";
		shader.setVec3((strUniform + ".position").c_str(), Position);
		shader.setVec3((strUniform + ".ambient").c_str(), Ambient);
		shader.setVec3((strUniform + ".diffuse").c_str(), Diffuse);
		shader.setVec3((strUniform + ".specular").c_str(), Specular);
		shader.setFloat((strUniform + ".constant").c_str(), Constant);
		shader.setFloat((strUniform + ".linear").c_str(), Linear);
		shader.setFloat((strUniform + ".quadratic").c_str(), Quadratic);
	}
};