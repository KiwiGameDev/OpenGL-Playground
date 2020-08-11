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
#include "pointlight.h"
#include "helpers.h"
#include "camera.h"
#include "assetmanager.h"

#define WIDTH 800
#define HEIGHT 800

Shader lightShader;
Shader shader;
std::vector<GameObject> gameObjects;
std::vector<PointLight> pointLights;
Camera* camera = new CameraPerspective((float)WIDTH / (float)HEIGHT, glm::vec3(0, 0, 5.0f));
//Camera* camera = new CameraOrthographic({ WIDTH * 0.01f, HEIGHT * 0.01f }, glm::vec3(0, 0, 5.0f));

void init(GLFWwindow* window)
{
	// Load shaders
	lightShader = Shader("lightingShader.vert", "lightShader.frag");
	shader = Shader("lightingShader.vert", "lightingShader.frag");

	// Load assets
	AssetManager& assetManager = AssetManager::getInstance();

	// Load gameobjects
	GameObject box = GameObject(assetManager.getVertexArrayObject(0));
	box.bind();
	box.textures.push_back(assetManager.getTexture(0));
	box.textures.push_back(assetManager.getTexture(1));
	box.textures.push_back(assetManager.getTexture(2));
	gameObjects.push_back(box);

	GameObject box2 = GameObject(assetManager.getVertexArrayObject(0));
	box2.bind();
	box2.textures.push_back(assetManager.getTexture(0));
	box2.textures.push_back(assetManager.getTexture(1));
	box2.textures.push_back(assetManager.getTexture(2));
	box2.Position = Vector3f(-4.0f, 4.0f, 0.0f);
	box2.Rotation = Vector3f(1.0f, 2.0f, 3.0f);
	gameObjects.push_back(box2);

	// Load lights
	PointLight pointLight(assetManager.getVertexArrayObject(0));
	pointLight.bind();
	pointLight.Scale = Vector3f(0.2f, 0.2f, 0.2f);
	pointLights.push_back(pointLight);
	pointLight.Position.x -= 2.0;
	pointLights.push_back(pointLight);

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

	// Point lights
	for (int i = 0; i < pointLights.size(); i++)
	{
		PointLight& pointLight = pointLights[i];

		pointLight.Position.x = sin(time) * 2 * pow(-1, i);
		pointLight.Position.y = -sin(time) * 2;
		pointLight.Position.z = cos(time) * 2 * pow(-1, i);

		pointLight.bind();
		pointLight.updateModelMatrix();
		glm::mat4 localToClip = pointLight.getLocalToClipMatrix(worldToClip);

		lightShader.use();
		lightShader.setMat4("u_localToClip", localToClip);

		pointLight.draw();
	}

	GameObject& box1 = gameObjects.front();
	box1.Rotation = Vector3f(0, time * 0.5f, 0);

	GameObject& box2 = gameObjects.back();
	box2.Rotation = Vector3f(time * 0.25f, time * 0.5f, time * 0.75f);

	// Common across all objects. Perhaps loop across all shaders
	shader.use();
	shader.setFloat("u_time", time);
	shader.setVec3("u_viewPos", camera->Position);

	for (GameObject& gameObject : gameObjects)
	{
		gameObject.bind();
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
		for (int i = 0; i < pointLights.size(); i++)
		{
			const PointLight& pointLight = pointLights[i];
			std::string strUniform = "u_pointLights[" + std::to_string(i) + "]";

			shader.setVec3((strUniform + ".position").c_str(), pointLight.Position);
			shader.setVec3((strUniform + ".ambient").c_str(), pointLight.Ambient);
			shader.setVec3((strUniform + ".diffuse").c_str(), pointLight.Diffuse);
			shader.setVec3((strUniform + ".specular").c_str(), pointLight.Specular);
			shader.setFloat((strUniform + ".constant").c_str(), pointLight.Constant);
			shader.setFloat((strUniform + ".linear").c_str(), pointLight.Linear);
			shader.setFloat((strUniform + ".quadratic").c_str(), pointLight.Quadratic);
		}

		// Spot Light
		shader.setVec3("u_spotLight.position", camera->Position);
		shader.setVec3("u_spotLight.direction", camera->Front);
		shader.setFloat("u_spotLight.cutoff", glm::cos(glm::radians(17.5f)));
		shader.setFloat("u_spotLight.outerCutoff", glm::cos(glm::radians(25.0f)));
		shader.setVec3("u_spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shader.setVec3("u_spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("u_spotLight.constant", 1.0f);
		shader.setFloat("u_spotLight.linear", 0.15f);
		shader.setFloat("u_spotLight.quadratic", 0.1f);

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