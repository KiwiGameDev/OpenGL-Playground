#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "image.h"
#include "vertexarrayobject.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

class CubeMap
{
public:
	CubeMap()
	{
		std::cout << "Invalid CubeMap instantiated!\n";
		textureID = 255;
	}

	CubeMap(const VertexArrayObject& vao, const Shader& shader, const std::vector<std::string>& faces)
		: vao(vao), shader(shader)
	{
		assert(faces.size() == 6);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		for (size_t i = 0; i < faces.size(); i++)
		{
			Image image = Image(faces[i], false);
			if (image.data)
			{
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB,
					image.width,
					image.height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					image.data
				);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void draw(glm::mat4 mvp) const
	{
		glDepthMask(GL_FALSE);
		shader.use();
		shader.setMat4("u_MVP", mvp);
		vao.bind();
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
	}

private:
	VertexArrayObject vao;
	Shader shader;
	unsigned int textureID;
};
