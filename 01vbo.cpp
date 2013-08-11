/* 
OpenGL examples - Vertex Buffer Object

setup and usage of vbo/vao with shaders
*/

#include "glutils.h"

#include <iostream>
#include <vector>
using namespace glm;

#define RED 1.0f, 0.4f, 0.4f, 1.0f
#define GREEN 0.4f, 1.0f, 0.4f, 1.0f
#define BLUE 0.4f, 0.4f, 1.0f, 1.0f
#define PURPLE 1.0f, 0.4f, 1.0f, 1.0f
#define ORANGE 1.0f, 1.0f, 0.4f, 1.0f
const GLfloat vertexData[] = {
	// Front
	-0.5f, -0.5f,  0.5f, RED,
	-0.5f,  0.5f,  0.5f, RED,
	0.5f,  0.5f,  0.5f, RED,
	0.5f, -0.5f,  0.5f, RED,

		// Back
	-0.5f, -0.5f, -0.5f, GREEN,
	0.5f, -0.5f, -0.5f, GREEN,
	0.5f,  0.5f, -0.5f, GREEN,
	-0.5f,  0.5f, -0.5f, GREEN,

	// Bottom
	-0.5f, -0.5f,  0.5f, PURPLE,
	0.5f, -0.5f,  0.5f, PURPLE,
	0.5f, -0.5f, -0.5f, PURPLE,
	-0.5f, -0.5f, -0.5f, PURPLE,

	// Top
	-0.5f,  0.5f,  0.5f, ORANGE,
	-0.5f,  0.5f, -0.5f, ORANGE,
	0.5f,  0.5f, -0.5f, ORANGE,
	0.5f,  0.5f,  0.5f, ORANGE,

		// Left
	-0.5f, -0.5f, -0.5f, BLUE,
	-0.5f,  0.5f, -0.5f, BLUE,
	-0.5f,  0.5f,  0.5f, BLUE,
	-0.5f, -0.5f,  0.5f, BLUE,

	// Right
	0.5f, -0.5f,  0.5f, BLUE,
	0.5f,  0.5f,  0.5f, BLUE,
	0.5f,  0.5f, -0.5f, BLUE,
	0.5f, -0.5f, -0.5f, BLUE
};

const GLushort indexData[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

GLuint vbo, vao, ibo;
GLuint program;
GLuint vsShader, fsShader;
GLuint attribPosition, attribColor;
GLuint uniformModel, uniformView, uniformProjection;

void initProgram()
{
	// load shader src
	std::string vsSrc, fsSrc;
	if(!readFile("data/simple.vs", vsSrc) ||
		!readFile("data/simple.fs", fsSrc))
		std::cerr<<"Failure reading shader data"<<std::endl;

	// compile shaders and link program
	vsShader = getShader(GL_VERTEX_SHADER, vsSrc);
	fsShader = getShader(GL_FRAGMENT_SHADER, fsSrc);
	program = getProgram(vsShader, fsShader);

	// get attrib/uniform locations
	attribPosition = glGetAttribLocation(program, "position");
	attribColor = glGetAttribLocation(program, "color");
	uniformModel = glGetUniformLocation(program, "model");
	uniformView = glGetUniformLocation(program, "view");
	uniformProjection = glGetUniformLocation(program, "projection");
}

void initBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create vertex buffer object to hold the vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * 4 * 7 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

	// create index buffer object to hold the index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLushort), indexData, GL_STATIC_DRAW);

	// enable and specify vertex format
	glEnableVertexAttribArray(attribPosition);
	glEnableVertexAttribArray(attribColor);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glVertexAttribPointer(attribColor, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// "unbind" vao
	glBindVertexArray(0);

	// "unbind" buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void render(double time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	mat4 model = mat4(1.0f);
	mat4 view = translate(0.0f, 0.0f, -3.0f) * rotateX(time * 2.0f) * rotateY(time);
	mat4 projection = perspective(45.0f, 640.0f / 480.0f, 0.1f, 10.0f);

	glUniform(uniformModel, model);
	glUniform(uniformView, view);
	glUniform(uniformProjection, projection);

	// draw 6 * 6 elements, starting at the 0th element in the ibo
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glfwSwapBuffers();
}

int main()
{
	int width = 640;
	int height = 480;

	if(!initGL("Vertex Buffer Objects", width, height, 24, 8, 4))
		exit(EXIT_FAILURE);

	initProgram();
	initBuffers();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	double targetFrameTime = 0.013; // 13ms
	while(glfwGetWindowParam(GLFW_OPENED))
	{
		double time = glfwGetTime();
		if(glfwGetKey(GLFW_KEY_ESC))
			glfwCloseWindow();
		
		double renderStart = glfwGetTime();
		render(time);
		double renderTime = glfwGetTime() - renderStart;

		// check for errors
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::cerr<<getErrorMessage(error)<<std::endl;
			glfwCloseWindow();
		}
		
		// a sort of framerate stabilizer
		if(renderTime < targetFrameTime)
			glfwSleep(targetFrameTime - renderTime);
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}