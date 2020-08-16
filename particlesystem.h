#pragma once
#include "particle.h"
#include "assetmanager.h"
#include "shadermanager.h"

#include <vector>

class ParticleSystem
{
public:
	std::vector<Particle> particles;

	ParticleSystem(int size)
	{
		AssetManager& assetManager = AssetManager::getInstance();
		ShaderManager& shaderManager = ShaderManager::getInstance();

		VertexArrayObject quad = assetManager.getVertexArrayObject("Quad");
		Shader particleShader = shaderManager.getShader("UnlitShader");
		Material particleMaterial;
		particleMaterial.setShader(particleShader);

		for (int i = 0; i < size; i++)
		{
			particles.push_back(Particle(quad, particleMaterial));
		}
	}

	void update(float deltaTime)
	{
		for (Particle& particle : particles)
			particle.update(deltaTime);
	}

	void draw(const glm::mat4& viewProjection)
	{
		for (const Particle& particle : particles)
		{
			particle.draw(viewProjection);
		}
	}
};