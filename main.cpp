#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <string>
#include <fstream>

#include "ShaderProgramAttachment.h"
#include "vector3.h"
#include "gameObject.h"
#include "helpers.h"

#define WIDTH 600
#define HEIGHT 600

GLuint renderingProgram;

std::vector<GameObject> gameObjects;

void init(GLFWwindow* window)
{
	renderingProgram = createShaderProgram();

	std::string basepath = "assets/";
	std::vector<std::string> inputFiles = {
		//basepath + "sphere.obj",
		basepath + "teapot.obj",
		//basepath + "bunny.obj",
	};

	for (std::string inputfile : inputFiles)
	{
		std::string err;
		std::vector<tinyobj::shape_t> tempShapes;
		std::vector<tinyobj::material_t> tempMaterials;

		tinyobj::LoadObj(tempShapes, tempMaterials, err, inputfile.c_str(), basepath.c_str());

		if (!err.empty())
			std::cerr << err << std::endl;
		else
			std::cout << "Loaded " << inputfile << " with shapes: " << tempShapes.size() << std::endl;

		for (const auto& shape : tempShapes)
		{
			gameObjects.emplace_back();
			GameObject& newGameObject = gameObjects.back();
			const auto& mesh = shape.mesh;

			// Positions
			const auto& pos = mesh.positions;
			for (int i = 0; i < pos.size();)
			{
				float x = pos[i++];
				float y = pos[i++];
				float z = pos[i++];
				newGameObject.vertexPositions.emplace_back(x, y, z);
			}

			// Normals
			const auto& norm = mesh.normals;
			for (int i = 0; i < norm.size();)
			{
				float x = norm[i++];
				float y = norm[i++];
				float z = norm[i++];
				newGameObject.vertexNormals.emplace_back(x, y, z);
			}

			// Color
			Vector3f baseColor = Vector3f(randf(), randf(), randf());
			newGameObject.vertexColors = newGameObject.vertexPositions;
			for (auto& color : newGameObject.vertexColors)
				color = baseColor;

			// Indices
			const auto& indices = mesh.indices;
			for (int i = 0; i < indices.size();)
			{
				unsigned int a = indices[i++];
				unsigned int b = indices[i++];
				unsigned int c = indices[i++];
				newGameObject.indices.emplace_back(a, b, c);
			}
		}
	}

	for (GameObject& gameObject : gameObjects)
	{
		GLuint bufferID;

		glGenVertexArrays(1, &gameObject.vaoID);
		glBindVertexArray(gameObject.vaoID);

		const auto& positions = gameObject.vertexPositions;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vector3f), &positions[0], GL_STATIC_DRAW);
		GLint vertexAttrib = glGetAttribLocation(renderingProgram, "v_vertex");
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& colors = gameObject.vertexColors;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3f), &colors[0], GL_STATIC_DRAW);
		GLint colorAttrib = glGetAttribLocation(renderingProgram, "v_color");
		glEnableVertexAttribArray(colorAttrib);
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& normals = gameObject.vertexNormals;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), &normals[0], GL_STATIC_DRAW);
		GLint normalAttrib = glGetAttribLocation(renderingProgram, "v_normal");
		glEnableVertexAttribArray(normalAttrib);
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& indices = gameObject.indices;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Vector3u), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(renderingProgram);

	// Camera stuff
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// Time
	float time = currentTime;
	GLuint timeLoc = glGetUniformLocation(renderingProgram, "u_time");
	glUniform1f(timeLoc, time);

	// Directional Light
	float directionalLight[3] = { 0, sin(time), cos(time) };
	GLuint lightLoc = glGetUniformLocation(renderingProgram, "u_directionalLight");
	glUniform3fv(lightLoc, 1, directionalLight);

	for (GameObject& gameObject : gameObjects)
	{
		glBindVertexArray(gameObject.vaoID);
		glUseProgram(renderingProgram);

		// Transform
		glm::mat4 transform = projection * view * gameObject.model;
		GLuint modelMatLoc = glGetUniformLocation(renderingProgram, "u_transform");
		glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawElements(GL_TRIANGLES, gameObject.indices.size() * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}

int main(void)
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OBJ Loader", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}