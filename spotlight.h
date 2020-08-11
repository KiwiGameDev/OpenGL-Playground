#pragma once

#include "shader.h"

class SpotLight
{
public:
	glm::vec3 Position;
	glm::vec3 Direction;

	float cutoff = 17.5f;
	float outerCutoff = 25.0f;

	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	float Constant;
	float Linear;
	float Quadratic;

	SpotLight()
	{
		Position = glm::vec3();
		Direction = glm::vec3();

		cutoff = 17.5f;
		outerCutoff = 25.0f;

		Ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		Specular = glm::vec3(1.0f, 1.0f, 1.0f);

		Constant = 1.0f;
		Linear = 0.15f;
		Quadratic = 0.1f;
	}

	void setOtherShaderUniforms(Shader shader)
	{
		shader.setVec3("u_spotLight.position", Position);
		shader.setVec3("u_spotLight.direction", Direction);
		shader.setFloat("u_spotLight.cutoff", glm::cos(glm::radians(cutoff)));
		shader.setFloat("u_spotLight.outerCutoff", glm::cos(glm::radians(outerCutoff)));
		shader.setVec3("u_spotLight.ambient", Ambient);
		shader.setVec3("u_spotLight.diffuse", Diffuse);
		shader.setVec3("u_spotLight.specular", Specular);
		shader.setFloat("u_spotLight.constant", 1.0f);
		shader.setFloat("u_spotLight.linear", 0.15f);
		shader.setFloat("u_spotLight.quadratic", 0.1f);
	}
};