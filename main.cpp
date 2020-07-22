#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "ShaderProgramAttachment.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

#define WIDTH 600
#define HEIGHT 600

//This is a variable that stores the program generated from the shader. Declare it here.
//This is where we declare the "OBJECT CONTAINERS." Declare it here.
//This is where we declare "OBJECTS" that contain data to be drawn. What is the data type to use and how many "OBJECTS"? Declare it here.
GLuint renderingProgram;

string basepath = "assets/";
vector<string> inputFiles = {
	basepath + "sphere.obj",
	basepath + "teapot.obj",
	basepath + "bunny.obj",
}; //enclose the obj filename.type in the string

vector<GLuint> vaos;
vector<tinyobj::shape_t> shapes; //This vector will hold the MESH data
vector<tinyobj::material_t> materials; //This vector will hold the MATERIAL data (WILL NOT BE USED YET)

void init(GLFWwindow* window)
{
	for (string inputfile : inputFiles)
	{
		string err;
		vector<tinyobj::shape_t> tempShapes;
		vector<tinyobj::material_t> tempMaterials;
		
		tinyobj::LoadObj(tempShapes, tempMaterials, err, inputfile.c_str(), basepath.c_str());
		
		if (!err.empty())
			std::cerr << err << std::endl;
		else
			std::cout << "Loaded " << inputfile << " with shapes: " << tempShapes.size() << std::endl;
	
		for (tinyobj::shape_t shape : tempShapes)
			shapes.push_back(shape);
		for (tinyobj::material_t material : tempMaterials)
			materials.push_back(material);
	}
	
	//Create a shader here and store it in a program. Check the ShaderProgram.h implementation
	//We now generate a VERTEX ARRAY that will contain our VERTEX BUFFERS.
	//Then bind the VERTEX ARRAY

	//What do we put inside the VERTEX ARRAY? Generate it here.
	//Before we can put data into the object above, what do we need to do? Do it here.
	//What data do we store into the object above? Check [T1] and the slides for the syntax.
	//We now store the object into the shader. But before doing that, what do we need to do?
	//Now, we communicate with the SHADER through the VERTEX ARRAY. Do that here.
	//It is a good practice to unbind the object above to save VRAM.

	//What is the third type of object? This helps us know the order of vertices. Generate it here.
	//Before we can put data into the object above, what do we need to do? Do it here.
	//What data do we store into the object above? Check [T1] for the syntax.
	//Is there a specific reason to communicate with the shader through this object? If not, do nothing.
	//What data do we store into this object?
	//It is a good practice to unbind the object to save VRAM.

	vector<vector<float>> colorsVec;
	for (int i = 0; i < shapes.size(); i++)
	{
		float c1, c2, c3;
		if (i == 0)
		{
			c1 = 0.1f;
			c2 = 0.4f;
			c3 = 0.69f;
		}
		else if (i == 1)
		{
			c1 = 0.6f;
			c2 = 0.1f;
			c3 = 0.6f;
		}
		else
		{
			c1 = 0.2f;
			c2 = 0.7f;
			c3 = 0.1f;
		}
		
		auto tempColors = shapes[i].mesh.positions;
		for (int j = 0; j < tempColors.size();)
		{
			tempColors[j++] = c1;
			tempColors[j++] = c2;
			tempColors[j++] = c3;
		}
		colorsVec.push_back(tempColors);
	}

	renderingProgram = createShaderProgram();

	vaos.resize(shapes.size());

	for (int i=0; i < shapes.size(); i++)
	{
		GLuint vao;
		GLuint bufferID;
		const auto& mesh = shapes[i].mesh;

		glGenVertexArrays(1, &vaos[i]);
		glBindVertexArray(vaos[i]);

		const auto& positions = mesh.positions;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);
		GLint vertexAttrib = glGetAttribLocation(renderingProgram, "v_vertex");
		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		const auto& colors = colorsVec[i];
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), &colors[0], GL_STATIC_DRAW);
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

		// Unbind buffers
		// Unbinding any buffer before the VAO will unbind the buffer from the VAO
		glBindVertexArray(0); // Unbind vao first
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

void display(GLFWwindow* window, double currentTime)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Use the program that we linked here.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(renderingProgram);

	//How will the GPU know which data to access? What contains all the BUFFERS? Bind it here.
	//Now that the data container, where is it specifically located? Bind it here.
	//The data has been bound to the buffer and communicated with the shader through a specific attribute. 
	//How do we tell the GPU which attribute this is? Check [T1] for syntax.
	//Another specific location of data that we need. This helps us know the order of vertices. Bind it here.
	//It's time to draw. What do we draw? Check [T1] for syntax.
	//It is a good practice to unbind the object to save VRAM.

	// Time
	float time = glfwGetTime();
	GLint timeID = glGetUniformLocation(renderingProgram, "time");
	glUniform1f(timeID, time);

	// Rotation
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, time, glm::vec3(0.0f, 1.0f, 0.0f));
	GLint transID = glGetUniformLocation(renderingProgram, "transform");
	glUniformMatrix4fv(transID, 1, GL_FALSE, glm::value_ptr(trans));

	// Directional Light
	float directionalLight[3] = { 0, sin(time), cos(time) };
	GLint lightID = glGetUniformLocation(renderingProgram, "directionalLight");
	glUniform3fv(lightID, 1, directionalLight);

	for (int i = 0; i < vaos.size(); i++)
	{
		glBindVertexArray(vaos[i]);
		glUseProgram(renderingProgram);
		
		glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, 0);
		
		// Unbind buffers
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

	init(window); //Initializes data to be drawn

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime()); //Draws per frame.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}