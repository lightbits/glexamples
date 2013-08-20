#include "glutils.h"
#include <memory>
#include <vector>
#include <iostream>

bool readFile(const char *filename, std::string &dest)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if(!in.is_open())
		return false;

	if(in.good())
	{
		in.seekg(0, std::ios::end);			// Set get position to end
		dest.resize(in.tellg());			// Resize string to support enough bytes
		in.seekg(0, std::ios::beg);			// Set get position to beginning
		in.read(&dest[0], dest.size());		// Read file to string
		in.close();
	}

	return true;
}

bool initGL(const char *title, int width, int height, int major, int minor,
int depth, int stencil, int fsaa, bool fullscreen)
{
	if(glfwInit() != GL_TRUE)
		return false;

	// Note that profiles were first introduced in GL 3.2,
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, 0);	// 0 = auto
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, major);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, minor);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, fsaa);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	if(glfwOpenWindow(width, height, 0, 0, 0, 0, depth, stencil, 
		fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
		return false;

	glfwSetWindowTitle(title);
	glfwSwapInterval(1); // vsync (experimental)

	// Note that this function fails if no GL context has been made current
	if(glload::LoadFunctions() == glload::LS_LOAD_FAILED)
		return false;
	
	std::cout<<"Debug context: "	<<(glfwGetWindowParam(GLFW_OPENGL_DEBUG_CONTEXT) ? "yes" : "no")<<std::endl;
	std::cout<<"HW accelerated: "	<<(glfwGetWindowParam(GLFW_ACCELERATED) ? "yes" : "no")<<std::endl;
	std::cout<<"Depth bits: "		<<glfwGetWindowParam(GLFW_DEPTH_BITS)<<std::endl;
	std::cout<<"Stencil bits: "		<<glfwGetWindowParam(GLFW_STENCIL_BITS)<<std::endl;
	std::cout<<"FSAA samples: "		<<glfwGetWindowParam(GLFW_FSAA_SAMPLES)<<std::endl;
	std::cout<<"Vendor: "			<<glGetString(GL_VENDOR)<<std::endl;
	std::cout<<"Renderer: "			<<glGetString(GL_RENDERER)<<std::endl;
	std::cout<<"GL ver.: "			<<glGetString(GL_VERSION)<<std::endl;
	std::cout<<"GLSL ver.: "		<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;

	return true;
}

GLuint getShader(GLenum shaderType, const std::string &shaderSrc)
{
	GLuint shader = glCreateShader(shaderType);
	const char *shaderSrcStr = shaderSrc.c_str();
	glShaderSource(shader, 1, &shaderSrcStr, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetShaderInfoLog(shader, length, NULL, &log[0]);
		std::cerr<<"Compile failure: "<<&log[0]<<std::endl;
	}

	return shader;
}

GLuint getProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader)
{
	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	if(geometryShader != 0) glAttachShader(program, geometryShader);

	glLinkProgram(program);

	// It is ok to detach shaders after linking
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	if(geometryShader != 0) glDetachShader(program, geometryShader);

	// Check for errors
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetProgramInfoLog(program, length, NULL, &log[0]);
		std::cerr<<"Linker failure: "<<std::endl<<&log[0]<<std::endl;
	}

	return program;
}

bool checkProgramLinkStatus(GLuint program)
{
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetProgramInfoLog(program, length, NULL, &log[0]);
		std::cerr<<"Linker failure: "<<std::endl<<&log[0]<<std::endl;
		return false;
	}

	return true;
}

bool checkShaderCompileStatus(GLuint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		std::vector<GLchar> log(length);
		glGetShaderInfoLog(shader, length, NULL, &log[0]);
		std::cerr<<"Compile failure: "<<&log[0]<<std::endl;
		return false;
	}

	return true;
}

bool loadTexture(GLuint &texture, const std::string &filename)
{
	try
	{
		// Allocate memory for images (deletes itself when no longer used)
		std::unique_ptr<glimg::ImageSet> imgset(glimg::loaders::stb::LoadFromFile(filename));
		texture = glimg::CreateTexture(imgset.get(), 0);
	}
	catch(glimg::loaders::stb::StbLoaderException &e)
	{
		texture = 0;
		std::cerr<<"Failure loading texture: "<<e.what()<<"("<<filename<<")"<<std::endl;
		return false;
	}

	return true;
}

bool loadTexture(GLuint &texture, const std::string &filename, GLenum target,
GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT)
{
	if(!loadTexture(texture, filename))
		return false;

	glBindTexture(target, texture);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glBindTexture(target, 0);

	return true;
}

GLuint createTexture2d(GLsizei width, GLsizei height, const void *data, GLenum dataType, GLenum format)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataType, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

void getTexture2dSize(GLuint texture, int &width, int &height)
{
	width = 0; 
	height = 0;
	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &height);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::mat4 rotateY(float t)
{
	float sint = sinf(t); float cost = cosf(t);
	glm::mat4 m(1.0f);
	m[0][0] = cost; m[0][2] = sint; m[2][0] = -sint; m[2][2] = cost;
	return m;
}

glm::mat4 rotateX(float t)
{
	float sint = sinf(t); float cost = cosf(t);
	glm::mat4 m(1.0f);
	m[1][1] = cost; m[1][2] = -sint; m[2][1] = sint; m[2][2] = cost;
	return m;
}

glm::mat4 rotateZ(float t)
{
	float sint = sinf(t); float cost = cosf(t);
	glm::mat4 m(1.0f);
	m[0][0] = cost; m[0][1] = sint; m[1][0] = -sint; m[1][1] = cost;
	return m;
}

glm::mat4 translate(float x, float y, float z)
{
	glm::mat4 m(1.0f);
	m[3][0] = x; m[3][1] = y; m[3][2] = z;
	return m;
}

glm::mat4 scale(float s)
{
	glm::mat4 m(1.0f);
	m[0][0] = s; m[1][1] = s; m[2][2] = s;
	return m;
}

glm::mat4 orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// map x, y and z from world coordinates
	// to the cube frustum extending from -1 to 1 in all axes

	// Map x linearly from [left, right] to [-1, 1]
	// Map y linearly from [bottom, top] to [-1, 1]
	// Map z linearly from [-zNear, -zFar] to [-1, 1]
	// The function to map linearly from one range to another is given by:
	//		y = ax + b
	// where:
	//		a = dy / dx = (y1 - y0) / (x1 - x0)
	//		x = the value to map
	//		y(x0) = ax0 + b = y0 => b = y0 - ax0
	//		y = ax + y0 - ax0 = (x - x0)a + y0
	//
	//		y = (x - x0)(y1 - y0) / (x1 - x0) + y0.

	// We encode translation in the matrix by using a 4x4 matrix and homogeneous coordinates
	// where w = 1.

	glm::mat4 result(1.0f);
	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = -2.0f / (zFar - zNear);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	result[3][2] = (zNear + zFar) / (zNear - zFar);

	return result;
}

void glUniform(GLint location, const glm::mat4 &mat) { glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
void glUniform(GLint location, const glm::mat3 &mat) { glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
void glUniform(GLint location, const glm::mat2 &mat) { glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat)); }
void glUniform(GLint location, const glm::vec4 &vec) { glUniform4f(location, vec.x, vec.y, vec.z, vec.w); }
void glUniform(GLint location, const glm::vec3 &vec) { glUniform3f(location, vec.x, vec.y, vec.z); }
void glUniform(GLint location, const glm::vec2 &vec) { glUniform2f(location, vec.x, vec.y); }
void glUniform(GLint location, GLfloat f) { glUniform1f(location, f); }
void glUniform(GLint location, GLint i) { glUniform1i(location, i); }
void glUniform(GLint location, GLuint i) { glUniform1ui(location, i); }