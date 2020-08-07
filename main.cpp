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

#include "shader.h"
#include "vector3.h"
#include "gameObject.h"
#include "helpers.h"
#include "camera.h"

#define WIDTH 800
#define HEIGHT 800



// TEMP
unsigned int lightVAO;
glm::vec3 lightPos(2.0f, 2.0f, 2.0f);


Shader lightShader;
Shader shader;
std::vector<GameObject> gameObjects;
Camera* camera = new CameraPerspective((float)WIDTH / (float)HEIGHT, glm::vec3(0, 0, 5.0f));
//Camera* camera = new CameraOrthographic({ WIDTH * 0.01f, HEIGHT * 0.01f }, glm::vec3(0, 0, 5.0f));

void init(GLFWwindow* window)
{
	lightShader = Shader("lightingShader.vert", "lightShader.frag");
	shader = Shader("lightingShader.vert", "lightingShader.frag");

	std::vector<std::string> objFiles =
	{
		"assets/default_cube.obj",
		//"assets/solid_snake.obj",
		//"assets/tank.obj",
	};

	for (auto& gameObject : loadGameObjects(objFiles))
	{
		gameObjects.push_back(gameObject);
	}

	for (GameObject& gameObject : gameObjects)
	{
		gameObject.init();
	}



	// TEMP
	GameObject& gameObject1 = gameObjects.back();
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, gameObject1.vertexPositionID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gameObject1.indicesID);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	float time = (float)currentTime;
	float directionalLight[3] = { 0, sin(time), cos(time) };

	// Camera stuff
	float radius = 2.0f;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 worldToClip = projection * view;

	for (GameObject& gameObject : gameObjects)
	{
		glBindVertexArray(gameObject.vaoID);

		glm::mat4 localToClip = worldToClip * gameObject.model;

		shader.use();
		shader.setMat4("u_model", gameObject.model);
		shader.setMat4("u_localToClip", localToClip);
		shader.setFloat("u_time", time);
		shader.setVec3("u_directionalLight", directionalLight);

		float objectColor[] = { 1.0f, 0.5f, 0.31f };
		float lightColor[] = { 1.0f, 1.0f, 1.0f };

		shader.setVec3("u_cameraPos", camera->Position);
		shader.setVec3("u_objectColor", objectColor);
		shader.setVec3("u_lightColor", lightColor);
		shader.setVec3("u_lightPos", lightPos);
		shader.setInt("u_shininess", 64);

		gameObject.bindTextures(shader.ID);

		glDrawElements(GL_TRIANGLES, gameObject.indices.size() * 3, GL_UNSIGNED_INT, 0);
	}




	// TEMP
	lightPos.x = sin(glfwGetTime()) * 2;
	lightPos.y = -sin(glfwGetTime()) * 2;
	lightPos.z = cos(glfwGetTime()) * 2;

	glBindVertexArray(lightVAO);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glm::mat4 localToClip = worldToClip * lightModel;

	lightShader.use();
	lightShader.setMat4("u_localToClip", localToClip);

	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);




	glBindVertexArray(0);
	glUseProgram(0);
}

void processKeyInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
}

void mouseCallback(GLFWwindow* window, double dPosX, double dPosY)
{
	static bool isFirstMouseCapture = true;
	static Vector2f lastMousePos = Vector2f(WIDTH * 0.5f, HEIGHT * 0.5f);

	float xPos = (float)dPosX;
	float yPos = (float)dPosY;

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

	camera->processMouseMovement(xOffset, yOffset);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera->processMouseScroll((float)yOffset);
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

	float lastFrameTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = (float)glfwGetTime();
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