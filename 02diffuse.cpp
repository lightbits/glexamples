/* 
OpenGL examples - Diffuse

Diffuse lighting shader
*/

#include "glutils.h"
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace glm;

struct Program
{
	GLuint handle;
	std::unordered_map<std::string, GLint> uniforms;
	std::unordered_map<std::string, GLint> attribs;
};

Program program;

GLuint vbo, vao, ibo;
GLuint vsShader, fsShader;
GLuint texture;

mat4 model = mat4(1.0f);
mat4 view = translate(0.0f, 0.0f, -3.0f) * rotateX(-0.59f) * rotateY(0.35f);
mat4 projection = glm::perspective(45.0f, 640.0f / 480.0f, 0.1f, 10.0f);

vec3 lightPos = vec3(0.0f, 0.0f, 3.0f);
vec4 lightColor = vec4(0.9f, 0.95f, 1.0f, 1.0f);
vec4 ambient = vec4(0.2f, 0.2f, 0.38f, 1.0f);

bool loadTextures()
{
	int width = 4;
	int height = 4;
	GLubyte *pixels = new GLubyte[width * height * 4];
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			GLubyte s = (x + y) % 2 == 0 ? 0 : 255;
			pixels[4 * (y * width + x) + 0] = s;
			pixels[4 * (y * width + x) + 1] = s;
			pixels[4 * (y * width + x) + 2] = s;
			pixels[4 * (y * width + x) + 3] = s;
		}
	}
	pixels[0] = 0; pixels[1] = 0; pixels[2] = 0; pixels[3] = 255;
	pixels[4] = 255; pixels[5] = 255; pixels[6] = 255; pixels[7] = 255;
	pixels[8] = 0; pixels[9] = 0; pixels[10] = 0; pixels[11] = 255;
	pixels[12] = 255; pixels[13] = 255; pixels[14] = 255; pixels[15] = 255;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] pixels;
	return true;
}

void initProgram()
{
	std::string vsSrc, fsSrc;
	if(!readFile("data/diffuse.vs", vsSrc) ||
		!readFile("data/diffuse.fs", fsSrc))
		std::cerr << "Failure reading shader data" << std::endl;

	vsShader = getShader(GL_VERTEX_SHADER, vsSrc);
	fsShader = getShader(GL_FRAGMENT_SHADER, fsSrc);
	program.handle = getProgram(vsShader, fsShader);

	program.attribs["position"] = glGetAttribLocation(program.handle, "position");
	program.attribs["normal"] = glGetAttribLocation(program.handle, "normal");
	program.attribs["texel"] = glGetAttribLocation(program.handle, "texel");

	program.uniforms["model"] = glGetUniformLocation(program.handle, "model");
	program.uniforms["view"] = glGetUniformLocation(program.handle, "view");
	program.uniforms["projection"] = glGetUniformLocation(program.handle, "projection");
	program.uniforms["lightPos"] = glGetUniformLocation(program.handle, "lightPos");
	program.uniforms["lightColor"] = glGetUniformLocation(program.handle, "lightColor");
	program.uniforms["ambient"] = glGetUniformLocation(program.handle, "ambient");
	program.uniforms["texBaseImage"] = glGetUniformLocation(program.handle, "texBaseImage");
}

void initBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const GLfloat vertices[] = {
		// xyz nnn uv
		// Front
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		// Back
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

		// Bottom
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		// Top
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		// Right
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};

	// create vertex buffer object to hold the vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	const GLushort indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	// create index buffer object to hold the index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// enable and specify vertex format
	glEnableVertexAttribArray(program.attribs["position"]);
	glEnableVertexAttribArray(program.attribs["normal"]);
	glEnableVertexAttribArray(program.attribs["texel"]);
	glVertexAttribPointer(program.attribs["position"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(program.attribs["normal"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(program.attribs["texel"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	// "unbind" vao
	glBindVertexArray(0);

	// "unbind" buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

double time0 = 0.0;
void update(double time)
{
	double dt = time - time0;

	model = rotateX(time) * rotateY(time);
	lightPos.y = 1.0f;
	lightPos.x = sinf(time * 2.0f);
	lightPos.z = cosf(time * 2.0f);

	time0 = time;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program.handle);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	glUniform(program.uniforms["model"], model);
	glUniform(program.uniforms["view"], view);
	glUniform(program.uniforms["projection"], projection);
	glUniform(program.uniforms["lightPos"], lightPos);
	glUniform(program.uniforms["lightColor"], lightColor);
	glUniform(program.uniforms["ambient"], ambient);
	glUniform(program.uniforms["texBaseImage"], 0); // texture unit 0 is for base image

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

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

	if(!initGL("Vertex Buffer Objects", width, height, 3, 1, 24, 8, 4, false))
		exit(EXIT_FAILURE);

	loadTextures();
	initProgram();
	initBuffers();

	glClearColor(0.55f, 0.59f, 0.95f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	while(glfwGetWindowParam(GLFW_OPENED))
	{
		double time = glfwGetTime();
		if(glfwGetKey(GLFW_KEY_ESC))
			glfwCloseWindow();

		update(time);
		
		render();

		// check for errors
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::cerr<<getErrorMessage(error)<<std::endl;
			std::cin.get();
			glfwCloseWindow();
		}
		
		// a sort of framerate stabilizer
		glfwSleep(0.013);
	}

	glDeleteTextures(1, &texture);
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