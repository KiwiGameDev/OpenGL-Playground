#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>

#include "gameObject.h"
#include "pointlight.h"
#include "directionallight.h"
#include "helpers.h"
#include "camera.h"
#include "assetmanager.h"
#include "shadermanager.h"

#define WIDTH 800
#define HEIGHT 800

Shader lightShader;
Shader mainShader;

std::vector<GameObject> gameObjects;
DirectionalLight directionalLight;
std::vector<PointLight> pointLights;
SpotLight spotLight;

Camera* camera = new CameraPerspective((float)WIDTH / (float)HEIGHT, glm::vec3(0, 0, 5.0f));
//Camera* camera = new CameraOrthographic({ WIDTH * 0.01f, HEIGHT * 0.01f }, glm::vec3(0, 0, 5.0f));

void init(GLFWwindow* window)
{
	// Load shaders
	ShaderManager& shaderManager = ShaderManager::getInstance();
	lightShader = shaderManager.getShader("LightShader");
	mainShader = shaderManager.getShader("LightingShader");

	// Load assets
	AssetManager& assetManager = AssetManager::getInstance();

	// Load gameobjects
	GameObject box = GameObject(assetManager.getVertexArrayObject(0), mainShader);
	box.bind();
	box.textures.push_back(assetManager.getTexture(0));
	box.textures.push_back(assetManager.getTexture(1));
	box.textures.push_back(assetManager.getTexture(2));
	gameObjects.push_back(box);

	GameObject box2 = GameObject(assetManager.getVertexArrayObject(0), mainShader);
	box2.bind();
	box2.textures.push_back(assetManager.getTexture(0));
	box2.textures.push_back(assetManager.getTexture(1));
	box2.textures.push_back(assetManager.getTexture(2));
	box2.Position = glm::vec3(-4.0f, 4.0f, 0.0f);
	box2.Rotation = glm::vec3(1.0f, 2.0f, 3.0f);
	gameObjects.push_back(box2);

	// Load lights
	PointLight pointLight(assetManager.getVertexArrayObject(0), lightShader);
	pointLight.bind();
	pointLight.Scale = glm::vec3(0.2f, 0.2f, 0.2f);
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

	ShaderManager::getInstance().updateShadersCommon(time, camera->Position);

	// Camera stuff
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProjection = projection * view;

	// Skybox
	glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
	glm::mat4 skyboxMVP = projection * skyboxView;
	CubeMap skybox = AssetManager::getInstance().getCubeMap(0);
	skybox.draw(skyboxMVP);

	// Update game objects
	GameObject& box1 = gameObjects.front();
	box1.Rotation = glm::vec3(0, time * 0.5f, 0);
	GameObject& box2 = gameObjects.back();
	box2.Rotation = glm::vec3(time * 0.25f, time * 0.5f, time * 0.75f);

	// Update lighting
	// Point lights
	for (int i = 0; i < pointLights.size(); i++)
	{
		PointLight& pointLight = pointLights[i];

		pointLight.Position.x = sin(time) * 2 * pow(-1, i);
		pointLight.Position.y = -sin(time) * 2;
		pointLight.Position.z = cos(time) * 2 * pow(-1, i);

		pointLight.bind();
		pointLight.updateModelMatrix();
		pointLight.updateShaderUniforms(viewProjection);

		pointLight.draw();
	}

	// Spot light
	spotLight.Position = camera->Position;
	spotLight.Direction = camera->Front;

	ShaderManager::getInstance().updateShadersLighting(directionalLight, pointLights, spotLight);

	for (GameObject& gameObject : gameObjects)
	{
		gameObject.bind();
		gameObject.updateModelMatrix();
		gameObject.updateShaderUniforms(viewProjection);
		gameObject.bindTextures();

		Shader shader = gameObject.shader;
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
	static glm::vec2 lastMousePos = glm::vec2(WIDTH * 0.5f, HEIGHT * 0.5f);

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