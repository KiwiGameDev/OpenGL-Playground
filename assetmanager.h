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
		loadObjFiles();
		loadTextureFiles();
		loadCubemaps();
	}

	void loadObjFiles()
	{
		std::string dirPath = "assets";
		for (const auto& entry : std::filesystem::directory_iterator(dirPath))
		{
			std::string err;
			std::vector<tinyobj::shape_t> tempShapes;
			std::vector<tinyobj::material_t> tempMaterials;

			std::string filePath = entry.path().string();

			if (filePath.find(".obj") == std::string::npos)
				continue;

			std::string name = filePath.substr(filePath.find_last_of('\\') + 1);
			name = name.substr(0, name.length() - 4);
			std::string obj = filePath;
			std::string mtl = filePath.substr(0, filePath.length() - 4) + ".mtl";

			tinyobj::LoadObj(tempShapes, tempMaterials, err, obj.c_str(), mtl.c_str());

			if (!err.empty())
				std::cerr << err << std::endl;
			else
				std::cout << "Loaded " << filePath
				<< " with shapes: " << tempShapes.size()
				<< std::endl;

			for (const auto& shape : tempShapes)
			{
				const auto& mesh = shape.mesh;
				const auto& pos = mesh.positions;
				const auto& norm = mesh.normals;
				const auto& tex = mesh.texcoords;
				const auto& indices = mesh.indices;
				VertexArrayObject newVAO = VertexArrayObject(pos, norm, tex, indices);
				vaos.insert(std::make_pair(name, newVAO));
			}
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
