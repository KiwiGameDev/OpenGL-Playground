#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>

#include "vector3.h"
#include "helpers.h"

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

std::vector<GameObject> loadGameObjects()
{
	std::vector<GameObject> gameObjects;

	std::string basepath = "assets/";
	std::vector<std::string> inputFiles = {
		basepath + "solid_snake.obj",
		basepath + "tank.obj",
	};

	for (std::string inputfile : inputFiles)
	{
		std::string err;
		std::vector<tinyobj::shape_t> tempShapes;
		std::vector<tinyobj::material_t> tempMaterials;

		tinyobj::LoadObj(tempShapes, tempMaterials, err, inputfile.c_str(), basepath.c_str());

		if (!err.empty())
			std::cerr << err << std::endl;
		else
			std::cout << "Loaded " << inputfile << " with shapes: " << tempShapes.size() << std::endl;

		for (const auto& shape : tempShapes)
		{
			gameObjects.emplace_back();
			GameObject& newGameObject = gameObjects.back();
			const auto& mesh = shape.mesh;

			// Positions
			const auto& pos = mesh.positions;
			for (size_t i = 0; i < pos.size();)
			{
				float x = pos[i++];
				float y = pos[i++];
				float z = pos[i++];
				newGameObject.vertexPositions.emplace_back(x, y, z);
			}

			// Normals
			const auto& norm = mesh.normals;
			for (size_t i = 0; i < norm.size();)
			{
				float x = norm[i++];
				float y = norm[i++];
				float z = norm[i++];
				newGameObject.vertexNormals.emplace_back(x, y, z);
			}

			// Color
			Vector3f baseColor = Vector3f(randf(), randf(), randf());
			newGameObject.vertexColors = newGameObject.vertexPositions;
			for (auto& color : newGameObject.vertexColors)
				color = baseColor;

			// Indices
			const auto& indices = mesh.indices;
			for (size_t i = 0; i < indices.size();)
			{
				unsigned int a = indices[i++];
				unsigned int b = indices[i++];
				unsigned int c = indices[i++];
				newGameObject.indices.emplace_back(a, b, c);
			}
		}
	}

	return gameObjects;
}