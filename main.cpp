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

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
		: x(0), y(0), z(0) { }

	Vector3(float x, float y, float z)
		: x(x), y(y), z(z) { }

	Vector3& operator+=(const Vector3& add)
	{
		this->x += add.x;
		this->y += add.y;
		this->z += add.z;
		return *this;
	}
};

struct GameObject
{
	GLuint vaoID;
	glm::mat4 transform;
	std::vector<Vector3> vertexPositions;
	std::vector<Vector3> vertexNormals;
	std::vector<Vector3> vertexColors;
	std::vector<unsigned int> indices;

	GameObject()
	{
		vaoID = -1;
		transform = glm::mat4(1.0f);
		vertexPositions = std::vector<Vector3>();
		vertexColors = std::vector<Vector3>();
		vertexNormals = std::vector<Vector3>();
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
		{
			const auto& mesh = shape.mesh;

			const auto& pos = mesh.positions;
			for (int i = 0; i < pos.size();)
			{
				float x = pos[i++];
				float y = pos[i++];
				float z = pos[i++];
				newGameObject.vertexPositions.emplace_back(x, y, z);
			}

			const auto& norm = mesh.normals;
			for (int i = 0; i < norm.size();)
			{
				float x = norm[i++];
				float y = norm[i++];
				float z = norm[i++];
				newGameObject.vertexNormals.emplace_back(x, y, z);
			}

			const auto& indices = mesh.indices;
			for (int i = 0; i < indices.size();)
				newGameObject.indices.emplace_back(indices[i++]);
		}

		gameObjects.push_back(newGameObject);
	}

	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject& gameObject = gameObjects[i];

		Vector3 baseColor;
		if (i == 0)
			baseColor = { 0.1f, 0.4f, 0.69f };
		else if (i == 1)
			baseColor = { 0.6f, 0.1f, 0.6f };
		else
			baseColor = { 0.2f, 0.7f, 0.1f };

		gameObject.vertexColors = gameObject.vertexPositions;
		for (auto& color : gameObject.vertexColors)
			color = baseColor;
	}

	for (GameObject& gameObject : gameObjects)
	{
		GLuint bufferID;

		glGenVertexArrays(1, &gameObject.vaoID);
		glBindVertexArray(gameObject.vaoID);

		const auto& positions = gameObject.vertexPositions;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vector3), &positions[0], GL_STATIC_DRAW);
		GLint vertexAttrib = glGetAttribLocation(renderingProgram, "v_vertex");
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& colors = gameObject.vertexColors;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3), &colors[0], GL_STATIC_DRAW);
		GLint colorAttrib = glGetAttribLocation(renderingProgram, "v_color");
		glEnableVertexAttribArray(colorAttrib);
		glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& normals = gameObject.vertexNormals;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3), &normals[0], GL_STATIC_DRAW);
		GLint normalAttrib = glGetAttribLocation(renderingProgram, "v_normal");
		glEnableVertexAttribArray(normalAttrib);
		glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		const auto& indices = gameObject.indices;
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

		glDrawElements(GL_TRIANGLES, gameObject.indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}

void input(GLFWwindow* window)
{
	const float ADD = 0.03f;
	glm::vec2 move = glm::vec2();

	if (glfwGetKey(window, GLFW_KEY_W))
		move.y += ADD;
	if (glfwGetKey(window, GLFW_KEY_S))
		move.y -= ADD;
	if (glfwGetKey(window, GLFW_KEY_A))
		move.x -= ADD;
	if (glfwGetKey(window, GLFW_KEY_D))
		move.x += ADD;

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