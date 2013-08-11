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

int main()
{
	int width = 640;
	int height = 480;
	
	if(!glfwInit())
		exit(EXIT_FAILURE);

	glfwOpenWindowHint(GLFW_OPENGL_PROFILE,	0);	// 0 lets the system choose the profile
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	if(glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW) != GL_TRUE)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowTitle("Hello World");

	// Note that this fails if no GL context has been made current
	if(glload::LoadFunctions() == glload::LS_LOAD_FAILED)
		return false;
	
	int running = GL_TRUE;
	while(running)
	{
		running = glfwGetWindowParam(GLFW_OPENED);
		double time = glfwGetTime();
		if(time > 3.0)
			running = false;

		glfwSwapBuffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}