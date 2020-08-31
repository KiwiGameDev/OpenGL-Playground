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
		ID = 4096;
		VertexPositionID = 4096;
		VertexNormalsID = 4096;
		VertexTexCoordsID = 4096;
		VertexTangentsID = 4096;
		IndicesID = 4096;
		IndicesSize = 0;
	}

	VertexArrayObject(const std::vector<float>& pos,
		const std::vector<float>& norm,
		const std::vector<float>& tex,
		const std::vector<unsigned int>& indices)
		: VertexArrayObject()
	{
		glGenVertexArrays(1, &ID);
		glBindVertexArray(ID);

		generateBufferLayout(VertexPositionID, pos, 0, 3);
		generateBufferLayout(VertexNormalsID, norm, 1, 3);
		generateBufferLayout(VertexTexCoordsID, tex, 2, 2);

		if (pos.size() > 0 && tex.size() > 0)
		{
			std::vector<glm::vec3> tangents(norm.size() / 3);
			std::vector<glm::vec3> bitangents(norm.size() / 3);

			for (int i = 0; i < indices.size(); i += 3)
			{
				int v1 = indices[i+0];
				int v2 = indices[i+1];
				int v3 = indices[i+2];

				glm::vec3 pos1 = glm::vec3(pos[v1], pos[v1 + 1], pos[v1 + 2]);
				glm::vec3 pos2 = glm::vec3(pos[v2], pos[v2 + 1], pos[v2 + 2]);
				glm::vec3 pos3 = glm::vec3(pos[v3], pos[v3 + 1], pos[v3 + 2]);

				glm::vec2 uv1 = glm::vec2(tex[v1], tex[v1 + 1]);
				glm::vec2 uv2 = glm::vec2(tex[v2], tex[v2 + 1]);
				glm::vec2 uv3 = glm::vec2(tex[v3], tex[v3 + 1]);

				// Edges of the triangle : position delta
				glm::vec3 deltaPos1 = pos2 - pos1;
				glm::vec3 deltaPos2 = pos3 - pos1;

				// UV delta
				glm::vec2 deltaUV1 = uv2 - uv1;
				glm::vec2 deltaUV2 = uv3 - uv1;

				float dirCorrection = (deltaUV2.x * deltaUV1.y - deltaUV2.y * deltaUV1.x) < 0.0f ? -1.0f : 1.0f;

				if (deltaUV1.x * deltaUV2.y == deltaUV1.y * deltaUV2.x)
				{
					deltaUV1.x = 0.0;
					deltaUV1.y = 1.0;
					deltaUV2.x = 1.0;
					deltaUV2.y = 0.0;
				}

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * (r * dirCorrection);
				glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

				tangents[v1] = (tangents[v1] * 1.05f) + tangent;
				tangents[v2] = (tangents[v2] * 1.05f) + tangent;
				tangents[v3] = (tangents[v3] * 1.05f) + tangent;
				bitangents[v1] += bitangent;
				bitangents[v2] += bitangent;
				bitangents[v3] += bitangent;
			}

			std::vector<glm::vec3> outTangents(norm.size() / 3);

			for (long i = 0; i < pos.size(); i += 3)
			{
				const glm::vec3& n = glm::vec3(pos[i + 0], pos[i + 1], pos[i + 2]);
				const glm::vec3& t = tangents[i / 3];

				// Gram-Schmidt orthogonalize
				outTangents[i / 3] = glm::normalize(t - n * glm::dot(n, t));
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

	bool isSimilar(glm::vec3 v1, glm::vec3 v2)
	{
		return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) < 0.001f;
	}

	bool isSimilar(glm::vec2 v1, glm::vec2 v2)
	{
		return (v1.x * v2.x + v1.y * v2.y) < 0.001f;
	}
};