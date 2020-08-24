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
		for (Shader& shader : lightingShaders)
		{
			shader.use();
			shader.setFloat("u_time", time);
			shader.setVec3("u_viewPos", viewPos);
		}
	}

	void updateShadersLighting(const DirectionalLight& dirLight,
							   const std::vector<PointLight>& pointLights,
							   const SpotLight& spotLight)
	{
		for (Shader& shader : lightingShaders)
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
		Shader textured = Shader("texturedShader.vert", "texturedShader.frag");
		Shader untextured = Shader("untexturedShader.vert", "untexturedShader.frag");
		Shader unlitShader = Shader("texturedShader.vert", "unlitShader.frag");
		Shader skyboxShader = Shader("skyboxShader.vert", "skyboxShader.frag");

		lightingShaders.push_back(textured);
		lightingShaders.push_back(untextured);

		shaderMap.insert(std::make_pair(
			"TexturedShader",
			textured
		));
		shaderMap.insert(std::make_pair(
			"UntexturedShader",
			untextured
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
	std::vector<Shader> lightingShaders;

public:
	ShaderManager(ShaderManager const&) = delete;
	void operator=(ShaderManager const&) = delete;
};