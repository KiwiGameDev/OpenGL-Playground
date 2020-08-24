#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "gameobject.h"
#include "shadermanager.h"
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
	
	const GameObject& getGameObject(const std::string& key)
	{
		auto itr = gameObjects.find(key);

		if (itr != gameObjects.end())
			return itr->second;

		std::cout << "Cannot find GameObject: " << key << '\n';
		return GameObject();
	}

	const VertexArrayObject& getVertexArrayObject(const std::string& key)
	{
		auto itr = vaos.find(key);

		if (itr != vaos.end())
			return itr->second;
		
		std::cout << "Cannot find VAO: " << key << '\n';
		return VertexArrayObject();
	}

	const Texture& getTexture(const std::string& key)
	{
		auto itr = textures.find(key);

		if (itr != textures.end())
			return itr->second;

		std::cout << "Cannot find Texture: " << key << '\n';
		return Texture();
	}

	const CubeMap& getCubeMap(const std::string& key)
	{
		auto itr = cubeMaps.find(key);

		if (itr != cubeMaps.end())
			return itr->second;

		std::cout << "Cannot find CubeMap: " << key << '\n';
		return CubeMap();
	}

private:
	const std::vector<std::string> cubemapFilepaths =
	{
		"assets/cubemap/right.jpg",
		"assets/cubemap/left.jpg",
		"assets/cubemap/top.jpg",
		"assets/cubemap/bottom.jpg",
		"assets/cubemap/front.jpg",
		"assets/cubemap/back.jpg"
	};

	std::unordered_map<std::string, GameObject> gameObjects;
	std::unordered_map<std::string, VertexArrayObject> vaos;
	std::unordered_map<std::string, Texture> textures;
	std::unordered_map<std::string, CubeMap> cubeMaps;

	AssetManager()
	{
		loadPresetVAOs();
		loadTextureFiles();
		loadObjFiles();
		loadCubemaps();
	}

	void loadObjFiles()
	{
		std::string dirPath = "assets";
		Shader texturedShader = ShaderManager::getInstance().getShader("TexturedShader");
		Shader untexturedShader = ShaderManager::getInstance().getShader("UntexturedShader");

		for (const auto& entry : std::filesystem::directory_iterator(dirPath))
		{
			std::string filePath = entry.path().string();

			if (filePath.find(".obj") == std::string::npos)
				continue;

			std::string name = filePath.substr(filePath.find_last_of('\\') + 1);
			name = name.substr(0, name.length() - 4);
			std::string obj = filePath;
			std::string mtl = filePath.substr(0, filePath.length() - 4) + ".mtl";

			std::string err;
			std::vector<tinyobj::shape_t> tinyShapes;
			std::vector<tinyobj::material_t> tinyMaterials;
			tinyobj::LoadObj(tinyShapes, tinyMaterials, err, obj.c_str(), mtl.c_str());

			if (!err.empty())
			{
				std::cerr << err << std::endl;
				continue;
			}
			else
			{
				std::cout << "Loaded " << filePath
						  << " with shapes: " << tinyShapes.size()
						  << std::endl;
			}

			std::vector<Material> tempMaterials;
			std::vector<Material> goMaterials;
			std::vector<VertexArrayObject> goVaos;

			for (const auto& material : tinyMaterials)
			{
				Material newMaterial;

				if (!material.diffuse_texname.empty())
				{
					newMaterial.setShader(texturedShader);

					newMaterial.setTexture(textures[material.diffuse_texname]);

					if (!material.bump_texname.empty())
						newMaterial.setTexture(textures[material.bump_texname]);
					else
						newMaterial.setTexture(textures["default_normal.jpg"]);

					if (!material.specular_texname.empty())
						newMaterial.setTexture(textures[material.specular_texname]);
					else
						newMaterial.setTexture(textures["default_specular.jpg"]);
				}
				else
				{
					newMaterial.setShader(untexturedShader);

					newMaterial.Diffuse[0] = material.diffuse[0];
					newMaterial.Diffuse[1] = material.diffuse[1];
					newMaterial.Diffuse[2] = material.diffuse[2];

					newMaterial.Specular[0] = material.specular[0];
					newMaterial.Specular[1] = material.specular[1];
					newMaterial.Specular[2] = material.specular[2];
				}

				tempMaterials.push_back(newMaterial);
			}

			for (const auto& shape : tinyShapes)
			{
				const auto& mesh = shape.mesh;
				const auto& pos = mesh.positions;
				const auto& norm = mesh.normals;
				const auto& tex = mesh.texcoords;
				const auto& indices = mesh.indices;

				VertexArrayObject newVAO = VertexArrayObject(pos, norm, tex, indices);

				vaos.insert(std::make_pair(name, newVAO));

				goVaos.push_back(newVAO);
				goMaterials.push_back(tempMaterials[mesh.material_ids[0]]); // Assume every face ID is equal
			}

			gameObjects.insert(std::make_pair(name, GameObject(goVaos, goMaterials)));
		}
	}

	void loadTextureFiles()
	{
		std::string path = "assets/textures";
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::string filePath = entry.path().string();
			std::string name = filePath.substr(filePath.find_last_of('\\') + 1);
			textures.insert(std::make_pair(name, Texture(filePath)));
		}
	}

	void loadCubemaps()
	{
		Shader skyboxShader = ShaderManager::getInstance().getShader("SkyboxShader");
		CubeMap cubeMap = CubeMap(vaos.at("box"), skyboxShader, cubemapFilepaths);
		cubeMaps.insert(std::make_pair("MainCubeMap", cubeMap));
	}

	void loadPresetVAOs()
	{
		// Quad
		std::vector<float> pos =
		{
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f
		};
		std::vector<float> norm =
		{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f
		};
		std::vector<unsigned int> indices =
		{
			0, 1, 2,
			1, 2, 3
		};
		vaos.insert(std::make_pair("Quad", VertexArrayObject(pos, norm, pos, indices)));
	}

public:
	AssetManager(AssetManager const&) = delete;
	void operator=(AssetManager const&) = delete;
};
