/* OpenGL examples - Vertex Buffer Object

setup and usage of vbo/vao with shaders

author: Simen Haugo
*/

#include <glload/gl_3_1_comp.h>			// OpenGL version 3.1, compatability profile
#include <glload/gll.hpp>				// C-style loading interface
#include <GL/glfw.h>

#include <glm/glm.hpp>					// OpenGL mathematics
#include <glm/gtc/type_ptr.hpp>			// for value_ptr(matrix)
#include <glm/gtc/matrix_transform.hpp> // for transformations like glm::ortho and glm::perspective

#include <iostream>
#include <vector>

const char *getErrorMessage(GLenum code)
{
	switch(code)
	{
	case 0: return "NO_ERROR";
	case 0x0500: return "INVALID_ENUM";
	case 0x0501: return "INVALID_VALUE";
	case 0x0502: return "INVALID_OPERATION";
	case 0x0503: return "STACK_OVERFLOW";
	case 0x0504: return "STACK_UNDERFLOW";
	case 0x0505: return "OUT_OF_MEMORY";
	case 0x0506: return "INVALID_FRAMEBUFFER_OPERATION";
	default: return "UNKNOWN";
	}
}

bool init(int width, int height, const char *title)
{	
	if(!glfwInit())
	{
		std::cerr<<"failed to initialize GLFW"<<std::endl;
		return false;
	}

	glfwOpenWindowHint(GLFW_OPENGL_PROFILE,	0);	// 0 lets the system choose the profile
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	// open window with default color bits, 24 depth bits and 8 stencil bits
	if(glfwOpenWindow(width, height, 0, 0, 0, 0, 24, 8, GLFW_WINDOW) != GL_TRUE)
	{
		std::cerr<<"failed to open window"<<std::endl;
		glfwTerminate();
		return false;
	}

	glfwSetWindowTitle(title);

	// Note that this fails if no GL context has been made current
	if(glload::LoadFunctions() == glload::LS_LOAD_FAILED)
	{
		std::cerr<<"failed to initialize glload"<<std::endl;
		return false;
	}

	std::cout<<"Running OpenGL ";
	std::cout<<glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR)<<".";
	std::cout<<glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR)<<std::endl;
	std::cout<<"Vendor: "<<glGetString(GL_VENDOR)<<std::endl;
	std::cout<<"Renderer: "<<glGetString(GL_RENDERER)<<std::endl;
	std::cout<<"Version: "<<glGetString(GL_VERSION)<<std::endl;
	std::cout<<"GLSL: "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;

	return true;
}

bool compileShader(GLuint &shader, GLenum shaderType, const char *src)
{
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	// check for errors
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetShaderInfoLog(shader, length, NULL, &log[0]);
		std::cerr<<&log[0]<<std::endl;
	}

	return status == GL_TRUE;
}

bool createProgram(GLuint &program, GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
{
	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	if(geometryShader != 0) glAttachShader(program, geometryShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	if(geometryShader != 0) glDetachShader(program, geometryShader);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetProgramInfoLog(program, length, NULL, &log[0]);
		std::cerr<<&log[0]<<std::endl;
	}

	return status == GL_TRUE;
}

int main()
{
	int width = 640;
	int height = 480;

	if(!init(width, height, "Vertex Buffer Objects"))
		exit(EXIT_FAILURE);

	GLuint vbo, vao;
	GLuint program;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER
	
	int running = GL_TRUE;
	while(running)
	{
		running = glfwGetWindowParam(GLFW_OPENED);

		// close on escape
		if(glfwGetKey(GLFW_KEY_ESC))
			running = GL_FALSE;

		// check for errors
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::cerr<<getErrorMessage(error)<<std::endl;
			running = GL_FALSE;
		}

		glfwSwapBuffers();
	}

	glfwCloseWindow();
	glfwTerminate();
	return EXIT_SUCCESS;
}