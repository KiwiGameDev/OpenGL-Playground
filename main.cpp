#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "ShaderProgramAttachment.h"

#define WIDTH 600
#define HEIGHT 600

GLuint renderingProgram;

std::string basepath = "assets/";
std::vector<std::string> inputFiles = {
	basepath + "sphere.obj",
	basepath + "teapot.obj",
	basepath + "bunny.obj",
};

struct GameObject
{
	GLuint vaoID;
	glm::mat4 transform;
	tinyobj::shape_t shape;
	std::vector<glm::vec3> colors;

	GameObject()
	{
		vaoID = -1;
		transform = glm::mat4(1.0f);
		shape = tinyobj::shape_t();
		colors = std::vector<glm::vec3>();
	}
};

std::vector<GameObject> gameObjects;

void init(GLFWwindow* window)
{
	renderingProgram = createShaderProgram();

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

		GameObject newGameObject;

		for (const tinyobj::shape_t& shape : tempShapes)
			newGameObject.shape = shape;

		gameObjects.push_back(newGameObject);
	}

	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject& gameObject = gameObjects[i];

		glm::vec3 baseColor;
		if (i == 0)
			baseColor = { 0.1f, 0.4f, 0.69f };
		else if (i == 1)
			baseColor = { 0.6f, 0.1f, 0.6f };
		else
			baseColor = { 0.2f, 0.7f, 0.1f };

		gameObject.colors = std::vector<glm::vec3>(gameObject.shape.mesh.positions.size() / 3);
		for (auto& color : gameObject.colors)
			color = baseColor;
	}

	for (GameObject& gameObject : gameObjects)
	{
		GLuint bufferID;
		const auto& mesh = gameObject.shape.mesh;

		glGenVertexArrays(1, &gameObject.vaoID);
		glBindVertexArray(gameObject.vaoID);

		const auto& positions = mesh.positions;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);
		GLint vertexAttrib = glGetAttribLocation(renderingProgram, "v_vertex");
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& colors = gameObject.colors;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3::value_type) * 3, &colors[0], GL_STATIC_DRAW);
		GLint colorAttrib = glGetAttribLocation(renderingProgram, "v_color");
		glEnableVertexAttribArray(colorAttrib);
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& normals = mesh.normals;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
		GLint normalAttrib = glGetAttribLocation(renderingProgram, "v_normal");
		glEnableVertexAttribArray(normalAttrib);
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& indices = mesh.indices;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(renderingProgram);

	// Time
	float time = glfwGetTime();
	GLint timeLoc = glGetUniformLocation(renderingProgram, "time");
	glUniform1f(timeLoc, time);

	// Directional Light
	float directionalLight[3] = { 0, sin(time), cos(time) };
	GLint lightLoc = glGetUniformLocation(renderingProgram, "directionalLight");
	glUniform3fv(lightLoc, 1, directionalLight);

	for (const GameObject& gameObject : gameObjects)
	{
		glBindVertexArray(gameObject.vaoID);
		glUseProgram(renderingProgram);

		// Transform
		GLint transformLoc = glGetUniformLocation(renderingProgram, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(gameObject.transform));

		glDrawElements(GL_TRIANGLES, gameObject.shape.mesh.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void input(GLFWwindow* window)
{
	glm::vec2 move = glm::vec2();

	if (glfwGetKey(window, GLFW_KEY_W))
		move.y += 0.03;
	if (glfwGetKey(window, GLFW_KEY_S))
		move.y -= 0.03;
	if (glfwGetKey(window, GLFW_KEY_A))
		move.x -= 0.03;
	if (glfwGetKey(window, GLFW_KEY_D))
		move.x += 0.03;

	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject& gameObject = gameObjects[i];
		glm::mat4& transform = gameObject.transform;

		if (i == 0)
			transform = glm::translate(transform, glm::vec3(move, 0));
		else if (i == 1)
			transform = glm::translate(transform, glm::vec3(move.y, move.x, 0));
		else
			transform = glm::translate(transform, glm::vec3(-move, 0));
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

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		input(window);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}