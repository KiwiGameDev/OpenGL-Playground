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
		"assets/box",
		"assets/box",
		//"assets/solid_snake",
		//"assets/tank",
	};

	for (auto& gameObject : loadGameObjects(objFiles))
	{
		gameObjects.push_back(gameObject);
	}

	for (GameObject& gameObject : gameObjects)
	{
		gameObject.init();
	}

	// Push textures
	GameObject& box = gameObjects.front();
	box.textures.push_back(Texture("assets/wood_box.png"));
	box.textures.push_back(Texture("assets/wood_box_specular.png"));
	GameObject& box2 = gameObjects[1];
	box2.textures.push_back(Texture("assets/wood_box.png"));
	box2.textures.push_back(Texture("assets/wood_box_specular.png"));
	box2.Position = Vector3f(-6.0f, 6.0f, 0.0f);
	box2.Rotation = Vector3f(1.0f, 2.0f, 3.0f);


	// TEMP
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, box.vertexPositionID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box.indicesID);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

void display(GLFWwindow* window, double currentTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Common variables
	float time = (float)currentTime;

	// Camera stuff
	float radius = 2.0f;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 worldToClip = projection * view;

	// Light
	lightPos.x = sin(time) * 2;
	lightPos.y = -sin(time) * 2;
	lightPos.z = cos(time) * 2;

	glBindVertexArray(lightVAO);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);
	lightModel = glm::scale(lightModel, glm::vec3(0.2f));
	glm::mat4 localToClip = worldToClip * lightModel;

	lightShader.use();
	lightShader.setMat4("u_localToClip", localToClip);

	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, 0);
	// Light END



	GameObject& box2 = gameObjects[1];
	box2.Rotation = Vector3f(time * 0.25f, time * 0.5f, time * 0.75f);




	// Common across all objects. Perhaps loop across all shaders
	shader.use();
	shader.setFloat("u_time", time);
	shader.setVec3("u_viewPos", camera->Position);

	for (GameObject& gameObject : gameObjects)
	{
		gameObject.bindVAO();
		gameObject.updateModelMatrix();
		gameObject.bindTextures(shader.ID);

		shader.use();
		shader.setMat4("u_model", gameObject.Model);
		shader.setMat4("u_localToClip", gameObject.getLocalToClipMatrix(worldToClip));

		// Directional light
		shader.setVec3("u_dirLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("u_dirLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("u_dirLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("u_dirLight.constant", 1.0f);

		// Point lights
		shader.setVec3("u_pointLights[0].position", lightPos);
		shader.setVec3("u_pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("u_pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("u_pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("u_pointLights[0].constant", 1.0f);
		shader.setFloat("u_pointLights[0].linear", 0.09f);
		shader.setFloat("u_pointLights[0].quadratic", 0.032f);

		// Spot Light
		shader.setVec3("u_spotLight.position", camera->Position);
		shader.setVec3("u_spotLight.direction", camera->Front);
		shader.setFloat("u_spotLight.cutoff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("u_spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		shader.setVec3("u_spotLight.diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("u_spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("u_spotLight.constant", 1.0f);
		shader.setFloat("u_spotLight.linear", 0.09f);
		shader.setFloat("u_spotLight.quadratic", 0.032f);

		shader.setVec3("u_material.ambient", 1.0f, 0.5f, 0.31f);
		shader.setVec3("u_material.diffuse", 1.0f, 0.5f, 0.31f);
		shader.setVec3("u_material.specular", 0.5f, 0.5f, 0.5f);
		shader.setFloat("u_material.shininess", 32.0f);

		gameObject.draw();
	}

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