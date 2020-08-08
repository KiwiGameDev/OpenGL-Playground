#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>

#include "texture.h"
#include "vector2.h"
#include "vector3.h"
#include "helpers.h"

struct GameObject
{
	GLuint vaoID;

	unsigned int vertexPositionID;
	unsigned int vertexColorsID;
	unsigned int vertexTexCoordsID;
	unsigned int vertexNormalsID;
	unsigned int indicesID;

	std::vector<Vector3f> vertexPositions;
	std::vector<Vector3f> vertexColors;
	std::vector<Vector2f> vertexTexCoords;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector3u> indices;

	std::vector<Texture> textures;

	glm::mat4 model;
	Vector3f position;
	Vector3f velocity;
	Vector3f rotation;

	GameObject()
	{
		vaoID = -1;

		vertexPositionID = 255;
		vertexColorsID = 255;
		vertexTexCoordsID = 255;
		vertexNormalsID = 255;
		indicesID = 255;

		vertexPositions = std::vector<Vector3f>();
		vertexColors = std::vector<Vector3f>();
		vertexTexCoords = std::vector<Vector2f>();
		vertexNormals = std::vector<Vector3f>();
		indices = std::vector<Vector3u>();

		textures = std::vector<Texture>();

		model = glm::mat4(1.0f);
		position = Vector3f();
		velocity = Vector3f();
		rotation = Vector3f();
	}

	GameObject(const tinyobj::shape_t& shape)
		: GameObject()
	{
		const auto& mesh = shape.mesh;

		// Positions
		const auto& pos = mesh.positions;
		for (size_t i = 0; i < pos.size();)
		{
			float x = pos[i++];
			float y = pos[i++];
			float z = pos[i++];
			vertexPositions.emplace_back(x, y, z);
		}

		// Normals
		const auto& norm = mesh.normals;
		for (size_t i = 0; i < norm.size();)
		{
			float x = norm[i++];
			float y = norm[i++];
			float z = norm[i++];
			vertexNormals.emplace_back(x, y, z);
		}

		// Texture coordinates
		const auto& texCoords = mesh.texcoords;
		for (size_t i = 0; i < texCoords.size();)
		{
			float x = texCoords[i++];
			float y = texCoords[i++];
			vertexTexCoords.emplace_back(x, y);
		}

		// Indices
		const auto& indices = mesh.indices;
		for (size_t i = 0; i < indices.size();)
		{
			unsigned int a = indices[i++];
			unsigned int b = indices[i++];
			unsigned int c = indices[i++];
			this->indices.emplace_back(a, b, c);
		}
	}

	void init()
	{
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		glGenBuffers(1, &vertexPositionID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexPositionID);
		glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(Vector3f), &vertexPositions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		if (vertexColors.size() > 0)
		{
			glGenBuffers(1, &vertexColorsID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexColorsID);
			glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(Vector3f), &vertexColors[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		if (vertexNormals.size() > 0)
		{
			glGenBuffers(1, &vertexNormalsID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexNormalsID);
			glBufferData(GL_ARRAY_BUFFER, vertexNormals.size() * sizeof(Vector3f), &vertexNormals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		if (vertexTexCoords.size() > 0)
		{
			glGenBuffers(1, &vertexTexCoordsID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexTexCoordsID);
			glBufferData(GL_ARRAY_BUFFER, vertexTexCoords.size() * sizeof(Vector2f), &vertexTexCoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);
		}

		glGenBuffers(1, &indicesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Vector3u), &indices[0], GL_STATIC_DRAW);
	}

	void randomizeVertexColors()
	{
		vertexColors = vertexPositions;
		for (auto& color : vertexColors)
			color = Vector3f(randf(), randf(), randf());
	}

	void bindTextures(unsigned int shaderID)
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].bindTexture(shaderID, i);
	}
};

std::vector<GameObject> loadGameObjects(std::vector<std::string> inputFiles)
{
	std::vector<GameObject> gameObjects;

	for (std::string inputfile : inputFiles)
	{
		std::string err;
		std::vector<tinyobj::shape_t> tempShapes;
		std::vector<tinyobj::material_t> tempMaterials;

		std::string obj = inputfile + ".obj";
		std::string mtl = inputfile + ".mtl";

		tinyobj::LoadObj(tempShapes, tempMaterials, err, obj.c_str(), mtl.c_str());

		if (!err.empty())
			std::cerr << err << std::endl;
		else
			std::cout << "Loaded " << inputfile << " with shapes: " << tempShapes.size() << std::endl;

		for (int i = 0; i < tempShapes.size(); i++)
			gameObjects.push_back(GameObject(tempShapes[0]));
	}

	return gameObjects;
}