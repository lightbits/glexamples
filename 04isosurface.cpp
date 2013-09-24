/* 
OpenGL examples - Smoothed isosurfaces

Triangulating an implicit surface using voxels and smoothing the result.
The potential field (f(x, y, z)) is sampled onto a cubical grid at a fixed resolution,
and later smoothed using trilinear interpolation
http://0fps.wordpress.com/2012/07/10/smooth-voxel-terrain-part-1/
http://0fps.wordpress.com/2012/07/12/smooth-voxel-terrain-part-2/

This example is unfinished (have not implemented smoothing yet)
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
int elementCount;

mat4 model;
mat4 view;
mat4 projection;

float surfaceFunction(float x, float y, float z)
{	
	/*vec4 p(x, y, z, 1.0f);
	p = rotateY(1.88f * y) * p;
	x = p.x;
	y = p.y;
	z = p.z;*/

	// Nordstrand's weird surface
	return 25.0f * (pow(x,3.0f)*(y+z) + pow(y,3.0f)*(x+z) + pow(z,3.0f)*(x+y)) +
        50.0f * (x*x*y*y + x*x*z*z + y*y*z*z) -
        125.0f * (x*x*y*z + y*y*x*z+z*z*x*y) +
        60.0f*x*y*z -
        4.0f*(x*y+x*z+y*z);

	//return x*x*x*x + y*y*y*y + z*z*z*z - 1.4f * (x*x + y*y + z*z) + 0.55f;

	//return x * x + y * y + z * z - 1.0f;
	//return y - x*x - z*z;
	//return x*x*x*x*x*x + y*y*y*y*y*y + z*z*z*z*z*z - 1.0f;
}

// Appends a clockwise oriented quad to the list of positions and indices
void addQuad(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec3 &v3,
std::vector<vec3> &positions, std::vector<GLushort> &indices)
{
	GLushort i = positions.size();
	positions.push_back(v0);
	positions.push_back(v1);
	positions.push_back(v2);
	positions.push_back(v3);
	indices.push_back(i + 0);
	indices.push_back(i + 1);
	indices.push_back(i + 2);
	indices.push_back(i + 2);
	indices.push_back(i + 3);
	indices.push_back(i + 0);
}

void polygonizeSurface(std::vector<vec3> &positions, std::vector<GLushort> &indices)
{
	int gridResolution = 128;
	float epsilon = 0.3f;
	float min = -1.5f;
	float max = 1.5f;
	float blockSize = (max - min) / float(gridResolution);

	positions.clear();
	indices.clear();
	for(int gx = 0; gx <= gridResolution; ++gx)
	{
		for(int gy = 0; gy <= gridResolution; ++gy)
		{
			for(int gz = 0; gz <= gridResolution; ++gz)
			{
				float x = min + (max - min) * (gx / float(gridResolution));
				float y = min + (max - min) * (gy / float(gridResolution));
				float z = min + (max - min) * (gz / float(gridResolution));
				// We approximate the level surface f(x, y, z) = 0 by
				// adding voxels where |f(x, y, z)| <= epsilon
				float f = std::abs(surfaceFunction(x, y, z));
				if(f <= epsilon)
				{
					float h = blockSize / 2.0f;
					// Front facing orientation is clockwise, note that we do some simple occlusion testing
					// Front and back
					if(std::abs(surfaceFunction(x, y, z + blockSize) > epsilon))
						addQuad(vec3(x-h, y-h, z+h), vec3(x-h, y+h, z+h), vec3(x+h, y+h, z+h), vec3(x+h, y-h, z+h), positions, indices);
					if(std::abs(surfaceFunction(x, y, z - blockSize) > epsilon))
						addQuad(vec3(x-h, y-h, z-h), vec3(x+h, y-h, z-h), vec3(x+h, y+h, z-h), vec3(x-h, y+h, z-h), positions, indices);

					// Top and bottom
					if(std::abs(surfaceFunction(x, y + blockSize, z) > epsilon))
						addQuad(vec3(x-h, y+h, z+h), vec3(x-h, y+h, z-h), vec3(x+h, y+h, z-h), vec3(x+h, y+h, z+h), positions, indices);
					if(std::abs(surfaceFunction(x, y - blockSize, z) > epsilon))
						addQuad(vec3(x-h, y-h, z+h), vec3(x+h, y-h, z+h), vec3(x+h, y-h, z-h), vec3(x-h, y-h, z-h), positions, indices);

					// Left and right
					if(std::abs(surfaceFunction(x - blockSize, y, z) > epsilon))
						addQuad(vec3(x-h, y-h, z-h), vec3(x-h, y+h, z-h), vec3(x-h, y+h, z+h), vec3(x-h, y-h, z+h), positions, indices);
					if(std::abs(surfaceFunction(x + blockSize, y, z) > epsilon))
					addQuad(vec3(x+h, y-h, z+h), vec3(x+h, y+h, z+h), vec3(x+h, y+h, z-h), vec3(x+h, y-h, z-h), positions, indices);
				}
			}
		}
	}

	std::cout<<"Generated isosurface ("<<positions.size()<<" vertices and "<<indices.size()<<" indices)"<<std::endl;
}

void computeSurfaceNormals(const std::vector<vec3> &positions, const std::vector<GLushort> &indices, 
std::vector<vec3> &normals, bool flip)
{
	normals = std::vector<vec3>(positions.size());
	for(int i = 0; i < indices.size() - 2; i += 3)
	{
		GLushort j0 = indices[i + 0];
		GLushort j1 = indices[i + 1];
		GLushort j2 = indices[i + 2];
		vec3 v0 = positions[j0];
		vec3 v1 = positions[j1];
		vec3 v2 = positions[j2];

		// Each triangle has the same normal, defined by the crossproduct of two edges
		vec3 n = normalize(cross(v1 - v0, v2 - v0));
		if(flip)
			n = -n;

		normals[j0] = n;
		normals[j1] = n;
		normals[j2] = n;
	}
}

void initProgram()
{
	std::string vsSrc, fsSrc;
	if(!readFile("data/isosurface.vs", vsSrc) ||
		!readFile("data/isosurface.fs", fsSrc))
		std::cerr<<"Failure reading shader data"<<std::endl;

	vsShader = getShader(GL_VERTEX_SHADER, vsSrc);
	fsShader = getShader(GL_FRAGMENT_SHADER, fsSrc);
	program.handle = getProgram(vsShader, fsShader);
	program.attribs["position"]	= glGetAttribLocation(program.handle, "position");
	program.attribs["normal"] = glGetAttribLocation(program.handle, "normal");
	program.uniforms["model"] = glGetUniformLocation(program.handle, "model");
	program.uniforms["view"] = glGetUniformLocation(program.handle, "view");
	program.uniforms["projection"] = glGetUniformLocation(program.handle, "projection");
	program.uniforms["white"] = glGetUniformLocation(program.handle, "white");
}

void initBuffers()
{
	std::vector<vec3> positions;
	std::vector<vec3> normals;
	std::vector<GLushort> indices;
	polygonizeSurface(positions, indices);
	computeSurfaceNormals(positions, indices, normals, true);
	elementCount = indices.size();
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer object to hold the vertex data
	GLsizeiptr b0 = positions.size() * sizeof(vec3);
	GLsizeiptr b1 = normals.size() * sizeof(vec3);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, b0 + b1, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,	b0,	&positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, b0, b1, &normals[0]);

	// Enable and specify vertex format
	glEnableVertexAttribArray(program.getAttribLoc("position"));
	glEnableVertexAttribArray(program.getAttribLoc("normal"));
	glVertexAttribPointer(program.getAttribLoc("position"),	3,	GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribPointer(program.getAttribLoc("normal"),	3,	GL_FLOAT, GL_FALSE, 0, (void*)(b0));

	// Create index buffer object to hold the index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	// "Unbind" vao and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

double time0 = 0.0;
int lastMouseX = 320;
int lastMouseY = 240;
float rotationSpeedX = 0.0f;
float rotationSpeedY = 0.0f;
float rotationX = 0.0f;
float rotationY = 0.0f;
void update(double time)
{
	double dt = time - time0;

	int mouseX, mouseY;
	glfwGetMousePos(&mouseX, &mouseY);
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{
		int dx = mouseX - lastMouseX;
		int dy = mouseY - lastMouseY;
		rotationSpeedY += float(dx) * 0.0005f;
		rotationSpeedX += float(dy) * 0.0005f;
	}

	rotationSpeedX = 0.95f * rotationSpeedX;
	rotationSpeedY = 0.95f * rotationSpeedY;
	rotationX += rotationSpeedX;
	rotationY += rotationSpeedY;
	lastMouseX = mouseX;
	lastMouseY = mouseY;

	model = rotateX(-rotationX) * rotateY(-rotationY);
	float zoom = float(glfwGetMouseWheel()) * 0.05f;
	view = translate(0.0f, 0.0f, -4.0f + zoom);
	projection = glm::perspective(45.0f, 640.0f / 480.0f, 0.1f, 10.0f);

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
	glUniform(program.uniforms["white"], 0.0f);

	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);

	// Draw wireframe
	glUniform(program.uniforms["white"], 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

	if(!initGL("Isosurface", width, height, 3, 1, 24, 8, 4, false))
		exit(EXIT_FAILURE);

	initProgram();
	initBuffers();	

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glClearDepth(1.0f);

	// Enable face culling (hide facets)
	/*glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);*/

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