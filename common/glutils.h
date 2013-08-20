/* 
OpenGL examples - GLutils

Functions that make life a little easier.
Among them, context initialization, compilation/linking of shaders/programs,
glUniform overloads, and simple matrix transformations
*/

#ifndef GL_UTILS_H
#define GL_UTILS_H
#include <glload/gl_3_1_comp.h>			// OpenGL version 3.1, compatability profile
#include <glload/gll.hpp>				// C-style loading interface
#include <GL/glfw.h>					// Context (GLFW 2.7)
#include <glm/glm.hpp>					// OpenGL mathematics
#include <glm/gtc/type_ptr.hpp>			// for value_ptr(matrix)
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho and glm::perspective
#include <glimg/glimg.h>				// for loadTexture
#include <fstream>						// for readFile
#include <string>

static const char *getErrorMessage(GLenum code)
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

/* read a file into the destination string.
	return true if successful.
	return false otherwise */
bool readFile(const char *filename, std::string &dest);

/* initialize OpenGL context and load GL functions */
bool initGL(const char *title, int width, int height, int major = 3, int minor = 1,
int depth = 24, int stencil = 8, int fsaa = 0, bool fullscreen = false);

/* compile a shader object of the given shaderType from the shaderSrc.
	return the shader if compilation was successful.
	return 0 otherwise */
GLuint getShader(GLenum shaderType, const std::string &shaderSrc);

/* compile a program using the vertex, fragment and optional geometry shader objects.
	return the program if link was successful.
	return 0 otherwise */
GLuint getProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0);

/* checks if any errors occured during linking
	return true if link was successful
	return false if an error occured */
bool checkProgramLinkStatus(GLuint program);

/* checks if any errors occured during compilation
	return true if compilation was successful
	return false if an error occured */
bool checkShaderCompileStatus(GLuint shader);

/* load a texture into the texture object.
	return true if successful.
	return false otherwise */
bool loadTexture(GLuint &texture, const std::string &filename);

/* load a texture into the texture object and apply the texture parameters.
	return true if successful.
	return false otherwise */
bool loadTexture(GLuint &texture, const std::string &filename, GLenum target,
GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT);

/* create a 2D texture from the pixel data of format format. The same format will be used in
	the internal texture object. Note that the width and height must be powers of two.
	
	For example, dataType: GL_FLOAT, format:GL_RGBA means each element is composed of 4 floats.
	dataType: GL_FLOAT, format: GL_DEPTH_COMPONENT means each element is composed of a single float,
	clamped to the range [0, 1]. (GL automatically converts the data to floats if this format is used).

	return the texture object if successful.
	return 0 otherwise */
GLuint createTexture2d(GLsizei width, GLsizei height, const void *data, GLenum dataType, GLenum format);

/* retrieves the dimensions of the 2D texture as represented internally in OpenGL */
void getTexture2dSize(GLuint texture, int &width, int &height);

/* Matrix operations */

glm::mat4 rotateY(float t);
glm::mat4 rotateX(float t);
glm::mat4 rotateZ(float t);
glm::mat4 translate(float x, float y, float z);
glm::mat4 scale(float s);
glm::mat4 orthographic(float left, float right, float bottom, float top, float zNear, float zFar);

/* Helper functions */

void glUniform(GLint location, const glm::mat4 &mat);
void glUniform(GLint location, const glm::mat3 &mat);
void glUniform(GLint location, const glm::mat2 &mat);
void glUniform(GLint location, const glm::vec4 &vec);
void glUniform(GLint location, const glm::vec3 &vec);
void glUniform(GLint location, const glm::vec2 &vec);
void glUniform(GLint location, GLfloat f);
void glUniform(GLint location, GLint i);

#endif