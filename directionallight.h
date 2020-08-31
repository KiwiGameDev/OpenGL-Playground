#pragma once
#include <glm/glm.hpp>

#include "shader.h"

class DirectionalLight
{
public:
	glm::vec3 Direction;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

	DirectionalLight()
	{
		Direction = glm::vec3(-0.2f, -1.0f, 0.3f);
		Ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		Diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
		Specular = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void setOtherShaderUniforms(Shader shader) const
	{
		shader.setVec3("u_dirLight.direction", Direction);
		shader.setVec3("u_dirLight.ambient", Ambient);
		shader.setVec3("u_dirLight.diffuse", Diffuse);
		shader.setVec3("u_dirLight.specular", Specular);
	}
};