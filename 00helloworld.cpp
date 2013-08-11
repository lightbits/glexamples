/* OpenGL examples - Hello World

create a window with GLFW and close it

author: Simen Haugo
*/

#include <glload/gl_3_1_comp.h>			// OpenGL version 3.1, compatability profile
#include <glload/gll.hpp>				// C-style loading interface
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>					// OpenGL mathematics
#include <glm/gtc/type_ptr.hpp>			// for value_ptr(matrix)
#include <glm/gtc/matrix_transform.hpp> // for transformations like glm::ortho and glm::perspective

#include <iostream>

int main()
{
	int width = 640;
	int height = 480;
	GLFWwindow *window;
	
	if(!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow *window;
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	// Note that this fails if no GL context has been made current
	if(glload::LoadFunctions() == glload::LS_LOAD_FAILED)
		return false;
	
	while(!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		if(time > 3.0)
			glfwSetWindowShouldClose(window, true);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}