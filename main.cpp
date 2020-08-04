#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <fstream>

#include "ShaderProgramAttachment.h"
#include "vector3.h"
#include "gameObject.h"
#include "helpers.h"
#include "camera.h"

#define WIDTH 800
#define HEIGHT 800

GLuint renderingProgram;

Camera camera(glm::vec3(0, 0, 1.0f));
std::vector<GameObject> gameObjects;

void init(GLFWwindow* window)
{
	renderingProgram = createShaderProgram();

	std::vector<std::string> objFiles =
	{
		//"assets/solid_snake.obj",
		"assets/tank.obj",
	};

	for (auto& gameObject : loadGameObjects(objFiles))
	{
		gameObject.randomizeVertexColors();
		gameObjects.push_back(gameObject);
	}

	Texture woodTexture = Texture("assets/wood_container.jpg");
	Texture faceTexture = Texture("assets/awesomeface.png");

	gameObjects.emplace_back();
	GameObject& tempGameObject1 = gameObjects.back();
	tempGameObject1.vertexPositions.emplace_back(-0.5f, -0.5f, -3);
	tempGameObject1.vertexPositions.emplace_back(0.5f, -0.5f, -3);
	tempGameObject1.vertexPositions.emplace_back(-0.5f, 0.5f, -3);
	tempGameObject1.vertexPositions.emplace_back(0.5f, 0.5f, -3);
	tempGameObject1.vertexTexCoords.emplace_back(0, 0);
	tempGameObject1.vertexTexCoords.emplace_back(1.0f, 0);
	tempGameObject1.vertexTexCoords.emplace_back(0, 1.0f);
	tempGameObject1.vertexTexCoords.emplace_back(1.0f, 1.0f);
	tempGameObject1.vertexColors.emplace_back(Vector3f(randf(), randf(), randf()));
	tempGameObject1.vertexColors.emplace_back(Vector3f(randf(), randf(), randf()));
	tempGameObject1.vertexColors.emplace_back(Vector3f(randf(), randf(), randf()));
	tempGameObject1.vertexColors.emplace_back(Vector3f(randf(), randf(), randf()));
	tempGameObject1.indices.emplace_back(0, 1, 2);
	tempGameObject1.indices.emplace_back(1, 2, 3);
	tempGameObject1.textures.emplace_back(woodTexture);
	tempGameObject1.textures.emplace_back(faceTexture);

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

		const auto& texCoords = gameObject.vertexTexCoords;
		if (texCoords.size() > 0)
		{
			glGenBuffers(1, &bufferID);
			glBindBuffer(GL_ARRAY_BUFFER, bufferID);
			glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(Vector2f), &texCoords[0], GL_STATIC_DRAW);
			GLint texCoordAttrib = glGetAttribLocation(renderingProgram, "v_texCoord");
			glEnableVertexAttribArray(texCoordAttrib);
			glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		const auto& colors = gameObject.vertexColors;
		if (colors.size() > 0)
		{
			glGenBuffers(1, &bufferID);
			glBindBuffer(GL_ARRAY_BUFFER, bufferID);
			glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3f), &colors[0], GL_STATIC_DRAW);
			GLint colorAttrib = glGetAttribLocation(renderingProgram, "v_color");
			glEnableVertexAttribArray(colorAttrib);
			glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		const auto& normals = gameObject.vertexNormals;
		if (normals.size() > 0)
		{
			glGenBuffers(1, &bufferID);
			glBindBuffer(GL_ARRAY_BUFFER, bufferID);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3f), &normals[0], GL_STATIC_DRAW);
			GLint normalAttrib = glGetAttribLocation(renderingProgram, "v_normal");
			glEnableVertexAttribArray(normalAttrib);
			glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		const auto& indices = gameObject.indices;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Vector3u), &indices[0], GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(renderingProgram);

	float time = static_cast<float>(currentTime);
	float directionalLight[3] = { 0, sin(time), cos(time) };

	// Camera stuff
	float radius = 2.0f;
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);

	for (GameObject& gameObject : gameObjects)
	{
		glBindVertexArray(gameObject.vaoID);
		glUseProgram(renderingProgram);

		// Transform
		glm::mat4 transform = projection * view * gameObject.model;
		GLuint modelMatLoc = glGetUniformLocation(renderingProgram, "u_transform");
		glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(transform));

		// Time
		GLuint timeLoc = glGetUniformLocation(renderingProgram, "u_time");
		glUniform1f(timeLoc, time);

		// Directional light
		GLuint lightLoc = glGetUniformLocation(renderingProgram, "u_directionalLight");
		glUniform3fv(lightLoc, 1, directionalLight);

		gameObject.bindTextures(renderingProgram);

		glDrawElements(GL_TRIANGLES, gameObject.indices.size() * 3, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

void processKeyInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	static bool isFirstMouseCapture = true;
	static Vector2f lastMousePos = Vector2f(WIDTH * 0.5f, HEIGHT * 0.5f);

	if (isFirstMouseCapture)
	{
		lastMousePos.x = xPos;
		lastMousePos.y = yPos;
		isFirstMouseCapture = false;
	}

	float xOffset = xPos - lastMousePos.x;
	float yOffset = lastMousePos.y - yPos;
	lastMousePos.x = xPos;
	lastMousePos.y = yPos;

	camera.processMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(yOffset);
}

int main(void)
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "S N A A A A K E ! ! !", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	float lastFrameTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		processKeyInput(window, deltaTime);
		display(window, currentTime);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}