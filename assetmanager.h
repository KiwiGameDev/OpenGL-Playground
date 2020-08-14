#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

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

	const VertexArrayObject& getVertexArrayObject(std::string key)
	{
		auto itr = vaos.find(key);

		if (itr != vaos.end())
			return itr->second;
		
		std::cout << "Cannot find VAO: " << key << '\n';
		return VertexArrayObject();
	}

	const Texture& getTexture(std::string key)
	{
		auto itr = textures.find(key);

		if (itr != textures.end())
			return itr->second;

		std::cout << "Cannot find Texture: " << key << '\n';
		return Texture();
	}

	const CubeMap& getCubeMap(std::string key)
	{
		auto itr = cubeMaps.find(key);

		if (itr != cubeMaps.end())
			return itr->second;

		std::cout << "Cannot find CubeMap: " << key << '\n';
		return CubeMap();
	}

private:
	const std::string objfiles[3] =
	{
		"assets/box",
		"assets/solid_snake",
		"assets/tank"
	};

	const TextureAsset textureAssets[3] =
	{
		{ "assets/textures/brickwall.jpg", TextureType::Diffuse },
		{ "assets/textures/brickwall_normal.jpg", TextureType::Normal },
		{ "assets/textures/brickwall_specular.jpg", TextureType::Specular }
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
		for (const std::string& filePath : objfiles)
		{
			std::string err;
			std::vector<tinyobj::shape_t> tempShapes;
			std::vector<tinyobj::material_t> tempMaterials;

			std::string name = filePath.substr(filePath.find_last_of('/') + 1);
			std::string obj = filePath + ".obj";
			std::string mtl = filePath + ".mtl";

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
		for (const TextureAsset& textureAsset : textureAssets)
		{
			std::string filePath = textureAsset.FilePath;
			std::string name = filePath.substr(filePath.find_last_of('/') + 1);
			textures.insert(std::make_pair(name, Texture(textureAsset)));
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
