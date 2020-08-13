#pragma once
#include "shader.h"
#include "directionallight.h"
#include "pointlight.h"
#include "spotlight.h"

#include <unordered_map>

class ShaderManager
{
public:
	static ShaderManager& getInstance()
	{
		static ShaderManager instance;
		return instance;
	}

	Shader getShader(const std::string& name)
	{
		return shaderMap[name];
	}

	void updateShadersCommon(float time, glm::vec3 viewPos)
	{
		for (Shader& shader : mainShaders)
		{
			shader.use();
			shader.setFloat("u_time", time);
			shader.setVec3("u_viewPos", viewPos);
		}
	}

	void updateShadersLighting(DirectionalLight dirLight, std::vector<PointLight> pointLights, SpotLight spotLight)
	{
		for (Shader& shader : mainShaders)
		{
			shader.use();
			dirLight.setOtherShaderUniforms(shader);
			for (int i = 0; i < pointLights.size(); i++)
				pointLights[i].setOtherShaderUniforms(shader, i);
			spotLight.setOtherShaderUniforms(shader);
		}
	}

private:
	ShaderManager()
	{
		Shader mainShader = Shader("lightingShader.vert", "lightingShader.frag");
		Shader unlitShader = Shader("lightingShader.vert", "unlitShader.frag");
		Shader skyboxShader = Shader("skyboxShader.vert", "skyboxShader.frag");

		mainShaders.push_back(mainShader);

		shaderMap.insert(std::make_pair(
			"LightingShader",
			mainShader
		));
		shaderMap.insert(std::make_pair(
			"UnlitShader",
			unlitShader
		));
		shaderMap.insert(std::make_pair(
			"SkyboxShader",
			skyboxShader
		));
	}

	std::unordered_map<std::string, Shader> shaderMap;
	std::vector<Shader> mainShaders;

public:
	ShaderManager(ShaderManager const&) = delete;
	void operator=(ShaderManager const&) = delete;
};