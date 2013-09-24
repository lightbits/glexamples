/* 
OpenGL examples - Mandelbrot
*/

#include "common/glutils.h"
#include "common/globj.h"
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace glm;

Program program;
GLuint vsShader;
GLuint fsShader;
GLuint vbo, vao, ibo;

void initProgram()
{
	std::string vsSrc, fsSrc;
	if(!readFile("data/mandelbrot.vs", vsSrc) ||
		!readFile("data/mandelbrot.fs", fsSrc))
		std::cerr<<"Failure reading shader data"<<std::endl;

	vsShader = getShader(GL_VERTEX_SHADER, vsSrc);
	fsShader = getShader(GL_FRAGMENT_SHADER, fsSrc);
	program.handle = getProgram(vsShader, fsShader);
	program.attribs["position"]	= glGetAttribLocation(program.handle, "position");
	program.uniforms["zoom"] = glGetUniformLocation(program.handle, "zoom");
	program.uniforms["offset"] = glGetUniformLocation(program.handle, "offset");
}

void initBuffers()
{	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLfloat quad[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f
	};

	// Create vertex buffer object to hold the vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	// Enable and specify vertex format
	glEnableVertexAttribArray(program.getAttribLoc("position"));
	glVertexAttribPointer(program.getAttribLoc("position"),	2,	GL_FLOAT, GL_FALSE, 0, (void*)(0));

	const GLushort indices[] = { 0, 1, 2, 2, 3, 0 };

	// Create index buffer object to hold the index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// "Unbind" vao and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

double time0 = 0.0;
int lastMouseX = 320;
int lastMouseY = 240;
float zoom = 0.0f;
float zoomSpeed = 0.0f;
vec2 offset = vec2(0.0f, 0.0f);
vec2 offsetSpeed = vec2(0.0f, 0.0f);
int mouseWheel0 = 0;
void update(double time)
{
	double dt = time - time0;

	int mouseX, mouseY;
	glfwGetMousePos(&mouseX, &mouseY);
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{
		int dx = mouseX - lastMouseX;
		int dy = mouseY - lastMouseY;
		offsetSpeed += vec2(dx * 0.005f, -dy * 0.005f);
	}

	int mouseWheel1 = glfwGetMouseWheel();
	int dw = mouseWheel1 - mouseWheel0;
	zoomSpeed += float(dw) * 0.0005f * (1.0f - zoom);
	mouseWheel0 = mouseWheel1;

	offsetSpeed *= 0.95f;
	offset = vec2(offset.x + offsetSpeed.x * dt, offset.y + offsetSpeed.y * dt);
	zoomSpeed *= 0.95f;
	zoom += zoomSpeed;

	lastMouseX = mouseX;
	lastMouseY = mouseY;

	time0 = time;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program.handle);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	glUniform(program.uniforms["zoom"], zoom);
	glUniform(program.uniforms["offset"], offset);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

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

	if(!initGL("Mandelbrot", width, height, 3, 1, 24, 8, 4, false))
		exit(EXIT_FAILURE);

	initProgram();
	initBuffers();

	glClearColor(1.0f, 1.0f, 1.0f, 0.73f);

	while(glfwGetWindowParam(GLFW_OPENED))
	{
		double time = glfwGetTime();
		update(time);
		render();

		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::cerr<<getErrorMessage(error)<<std::endl;
			std::cin.get();
			glfwCloseWindow();
		}
		
		// Let cpu get some sleep
		glfwSleep(0.013);
	}

	glDeleteShader(vsShader);
	glDeleteShader(fsShader);
	glDeleteProgram(program.handle);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();
	return EXIT_SUCCESS;
}