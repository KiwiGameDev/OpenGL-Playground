#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "shader.h"
#include "AssetManager.h"
#include "transformable.h"
#include "drawable.h"

class GameObject : public Transformable, public Drawable
{
public:
	std::vector<Texture> textures;

	GameObject(VertexArrayObject vao, Shader shader)
		: Drawable(vao, shader) { }

	void updateShaderUniforms(glm::mat4 viewProjection) const
	{
		shader.setMat4("u_model", Model);
		shader.setMat4("u_localToClip", getMVP(viewProjection));
	}

	void bindTextures() const
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].bindTexture(shader.ID, i);
	}
};
