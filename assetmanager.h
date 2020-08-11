#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

#include "vertexarrayobject.h"
#include "texture.h"
#include "cubemap.h"

class AssetManager
{
public:
	static AssetManager& getInstance()
	{
		static AssetManager instance;
		return instance;
	}

	const VertexArrayObject& getVertexArrayObject(int index)
	{
		return vaos[index];
	}

	const Texture& getTexture(int index)
	{
		return textures[index];
	}

	const CubeMap& getCubeMap(int index)
	{
		return cubeMaps[index];
	}

private:
	const std::string objfiles[1] =
	{
		"assets/box",
		//"assets/solid_snake",
		//"assets/tank"
	};

	const TextureAsset textureAssets[3] =
	{
		{ "assets/brickwall.jpg", TextureType::Diffuse },
		{ "assets/brickwall_normal.jpg", TextureType::Normal },
		{ "assets/brickwall_specular.jpg", TextureType::Specular }
	};
	
	const std::vector<std::string> cubemapFilepaths =
	{
		"assets/cubemap/right.jpg",
		"assets/cubemap/left.jpg",
		"assets/cubemap/top.jpg",
		"assets/cubemap/bottom.jpg",
		"assets/cubemap/front.jpg",
		"assets/cubemap/back.jpg"
	};

	std::vector<VertexArrayObject> vaos;
	std::vector<Texture> textures;
	std::vector<CubeMap> cubeMaps;

	AssetManager()
	{
		loadObjFiles();
		loadTextureFiles();
		loadCubemaps();
	}

	void loadObjFiles()
	{
		for (const std::string& inputfile : objfiles)
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
				std::cout << "Loaded " << inputfile
				<< " with shapes: " << tempShapes.size()
				<< std::endl;

			for (const auto& tempShape : tempShapes)
			{
				loadShape(tempShape);
			}
		}
	}

	void loadShape(const tinyobj::shape_t& shape)
	{
		const auto& mesh = shape.mesh;
		const auto& pos = mesh.positions;
		const auto& norm = mesh.normals;
		const auto& tex = mesh.texcoords;
		const auto& indices = mesh.indices;

		VertexArrayObject obj;
		glGenVertexArrays(1, &obj.ID);
		glBindVertexArray(obj.ID);

		glGenBuffers(1, &obj.VertexPositionID);
		glBindBuffer(GL_ARRAY_BUFFER, obj.VertexPositionID);
		glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(float), &pos[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		if (norm.size() > 0)
		{
			glGenBuffers(1, &obj.VertexNormalsID);
			glBindBuffer(GL_ARRAY_BUFFER, obj.VertexNormalsID);
			glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(float), &norm[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		if (tex.size() > 0)
		{
			glGenBuffers(1, &obj.VertexTexCoordsID);
			glBindBuffer(GL_ARRAY_BUFFER, obj.VertexTexCoordsID);
			glBufferData(GL_ARRAY_BUFFER, tex.size() * sizeof(float), &tex[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		if (norm.size() > 0 && tex.size() > 0)
		{
			std::vector<glm::vec3> tangents;

			for (int i = 0; i < pos.size() / 9; i++)
			{
				int j = i * 9;
				glm::vec3 pos1 = glm::vec3(pos[j + 0], pos[j + 1], pos[j + 2]);
				glm::vec3 pos2 = glm::vec3(pos[j + 3], pos[j + 4], pos[j + 5]);
				glm::vec3 pos3 = glm::vec3(pos[j + 6], pos[j + 7], pos[j + 8]);

				j = i * 6;
				glm::vec2 uv1 = glm::vec2(tex[j + 0], tex[j + 1]);
				glm::vec2 uv2 = glm::vec2(tex[j + 2], tex[j + 3]);
				glm::vec2 uv3 = glm::vec2(tex[j + 4], tex[j + 5]);

				// Edges of the triangle : position delta
				glm::vec3 deltaPos1 = pos2 - pos1;
				glm::vec3 deltaPos2 = pos3 - pos1;

				// UV delta
				glm::vec2 deltaUV1 = uv2 - uv1;
				glm::vec2 deltaUV2 = uv3 - uv1;

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y * r;

				tangents.push_back(tangent);
				tangents.push_back(tangent);
				tangents.push_back(tangent);
			}

			glGenBuffers(1, &obj.VertexTangentsID);
			glBindBuffer(GL_ARRAY_BUFFER, obj.VertexTangentsID);
			glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);
		}

		glGenBuffers(1, &obj.IndicesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.IndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		obj.IndicesSize = indices.size();

		vaos.push_back(obj);
	}

	void loadTextureFiles()
	{
		for (const TextureAsset& textureAsset : textureAssets)
			textures.push_back(Texture(textureAsset));
	}

	void loadCubemaps()
	{
		cubeMaps.emplace_back(vaos.front(), cubemapFilepaths);
	}

public:
	AssetManager(AssetManager const&) = delete;
	void operator=(AssetManager const&) = delete;
};
