/* OpenGL examples - Hello World

create a window with GLFW and close it

author: Simen Haugo
*/

#include <glload/gl_3_1_comp.h>			// OpenGL version 3.1, compatability profile
#include <glload/gll.hpp>				// C-style loading interface
#include <GL/glfw.h>

#include <glm/glm.hpp>					// OpenGL mathematics
#include <glm/gtc/type_ptr.hpp>			// for value_ptr(matrix)
#include <glm/gtc/matrix_transform.hpp> // for transformations like glm::ortho and glm::perspective

#include <iostream>

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

int main()
{
	int width = 640;
	int height = 480;
	
	if(!glfwInit())
	{
		std::cerr<<"failed to initialize GLFW"<<std::endl;
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle("Hello World");

	// Note that this fails if no GL context has been made current
	if(glload::LoadFunctions() == glload::LS_LOAD_FAILED)
	{
		std::cerr<<"failed to initialize glload"<<std::endl;
		exit(EXIT_FAILURE);
	}
	
	int running = GL_TRUE;
	while(running)
	{
		running = glfwGetWindowParam(GLFW_OPENED);

		// close on escape
		if(glfwGetKey(GLFW_KEY_ESC))
			running = GL_FALSE;

		// drawing and stuff

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