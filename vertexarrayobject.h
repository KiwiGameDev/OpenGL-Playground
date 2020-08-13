#pragma once
#include <GL/glew.h>
#include <glfw/glfw3.h>

class VertexArrayObject
{
public:
	unsigned int ID;
	unsigned int VertexPositionID;
	unsigned int VertexNormalsID;
	unsigned int VertexTexCoordsID;
	unsigned int VertexTangentsID;
	unsigned int IndicesID;
	size_t IndicesSize;

	VertexArrayObject()
	{
		ID = 255;
		VertexPositionID = 255;
		VertexNormalsID = 255;
		VertexTexCoordsID = 255;
		VertexTangentsID = 255;
		IndicesID = 255;
	}

	VertexArrayObject(std::vector<float> pos, std::vector<float> norm, std::vector<float> tex, std::vector<unsigned int> indices)
		: VertexArrayObject()
	{
		glGenVertexArrays(1, &ID);
		glBindVertexArray(ID);

		generateBufferLayout(VertexPositionID, pos, 0, 3);
		generateBufferLayout(VertexNormalsID, norm, 1, 3);
		generateBufferLayout(VertexTexCoordsID, tex, 2, 2);

		if (pos.size() > 0 && tex.size() > 0)
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

			generateBufferLayout(VertexTangentsID, tangents, 3, 3);
		}

		IndicesSize = indices.size();

		glGenBuffers(1, &IndicesID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndicesSize * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	void bind() const
	{
		glBindVertexArray(ID);
	}

	void draw() const
	{
		glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, 0);
	}

private:
	template<typename T>
	void generateBufferLayout(unsigned int& ID, const std::vector<T>& buffer, int location, int size)
	{
		if (buffer.size() == 0)
			return;

		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), &buffer[0], GL_STATIC_DRAW);
		glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(location);
	}
};