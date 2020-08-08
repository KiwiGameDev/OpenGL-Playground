#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

#include "texture.h"
#include "vertexarrayobject.h"


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

private:
	const std::string objfiles[3] =
	{
		"assets/box",
		"assets/solid_snake",
		"assets/tank",
	};

	const std::string textureFiles[2] =
	{
		"assets/wood_box.png",
		"assets/wood_box_specular.png"
	};

	std::vector<VertexArrayObject> vaos;
	std::vector<Texture> textures;

    AssetManager()
    {
		loadObjFiles();
		loadTextureFiles();
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
				loadShape(tempShape);
		}
	}

	void loadShape(const tinyobj::shape_t& shape)
	{
		const tinyobj::mesh_t& mesh = shape.mesh;

		VertexArrayObject obj;
		glGenVertexArrays(1, &obj.ID);
		glBindVertexArray(obj.ID);

		glGenBuffers(1, &obj.VertexPositionID);
		glBindBuffer(GL_ARRAY_BUFFER, obj.VertexPositionID);
		glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(float), &mesh.positions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		if (mesh.normals.size() > 0)
		{
			glGenBuffers(1, &obj.VertexNormalsID);
			glBindBuffer(GL_ARRAY_BUFFER, obj.VertexNormalsID);
			glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), &mesh.normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}
		
		if (mesh.texcoords.size() > 0)
		{
			glGenBuffers(1, &obj.VertexTexCoordsID);
			glBindBuffer(GL_ARRAY_BUFFER, obj.VertexTexCoordsID);
			glBufferData(GL_ARRAY_BUFFER, mesh.texcoords.size() * sizeof(float), &mesh.texcoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
		}

		glGenBuffers(1, &obj.IndicesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.IndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

		obj.IndicesSize = mesh.indices.size();

		vaos.push_back(obj);
	}

	void loadTextureFiles()
	{
		for (const std::string& inputfile : textureFiles)
		{
			textures.push_back(Texture(inputfile));
		}
	}

public:
    AssetManager(AssetManager const&) = delete;
    void operator=(AssetManager const&) = delete;
};
