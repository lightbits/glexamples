/* 
OpenGL examples - Patched sphere

Generate a sphere mesh by normalizing a cube
http://www.iquilezles.org/www/articles/patchedsphere/patchedsphere.htm
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

void generateSphereNormal(std::vector<vec3> &positions, std::vector<GLushort> &indices)
{
	static const float PI = 3.1415926535f;
	static const float TWO_PI = 6.2831853071f;
	float r = 2.0f;
	float step = PI / 8.0f;
	for(float theta = 0.0f; theta <= TWO_PI - step * 0.95f; theta += step)
	{
		for(float phi = 0.0f; phi <= PI - step * 0.95f; phi += step)
		{
			std::size_t i = positions.size();
			vec3 v0 = vec3(r*sinf(phi)*sinf(theta), r*cosf(phi), r*sinf(phi)*cosf(theta));
			vec3 v1 = vec3(r*sinf(phi)*sinf(theta + step), r*cosf(phi), r*sinf(phi)*cosf(theta + step));
			vec3 v2 = vec3(r*sinf(phi + step)*sinf(theta + step), r*cosf(phi + step), r*sinf(phi + step)*cosf(theta + step));
			vec3 v3 = vec3(r*sinf(phi + step)*sinf(theta), r*cosf(phi + step), r*sinf(phi + step)*cosf(theta));
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
	}
}

void generateSpherePatched(std::vector<vec3> &positions, std::vector<GLushort> &indices)
{
	float r = 2.0f;
	int m = 5; // squares per face
	for(int i = 0; i <= m - 1; i += 1)
	{
		for(int j = 0; j <= m - 1; j += 1)
		{
			float sa = -1.0f + 2.0f * (i / float(m));
			float sb = -1.0f + 2.0f * ((i + 1) / float(m));
			float ta = -1.0f + 2.0f * (j / float(m));
			float tb = -1.0f + 2.0f * ((j + 1) / float(m));
			GLushort i = positions.size();
			positions.push_back(normalize(vec3(sa,	tb,	1.0f)));
			positions.push_back(normalize(vec3(sb,	tb,	1.0f)));
			positions.push_back(normalize(vec3(sb,	ta,	1.0f)));
			positions.push_back(normalize(vec3(sa,	ta,	1.0f)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
			positions.push_back(normalize(vec3(sa,	ta,	-1.0f)));
			positions.push_back(normalize(vec3(sb,	ta,	-1.0f)));
			positions.push_back(normalize(vec3(sb,	tb,	-1.0f)));
			positions.push_back(normalize(vec3(sa,	tb,	-1.0f)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
			positions.push_back(normalize(vec3(sa,	1.0f, ta)));
			positions.push_back(normalize(vec3(sb,	1.0f, ta)));
			positions.push_back(normalize(vec3(sb,	1.0f, tb)));
			positions.push_back(normalize(vec3(sa,	1.0f, tb)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
			positions.push_back(normalize(vec3(sa, -1.0f, tb)));
			positions.push_back(normalize(vec3(sb, -1.0f, tb)));
			positions.push_back(normalize(vec3(sb, -1.0f, ta)));
			positions.push_back(normalize(vec3(sa, -1.0f, ta)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
			positions.push_back(normalize(vec3(-1.0f, sa, ta)));
			positions.push_back(normalize(vec3(-1.0f, sb, ta)));
			positions.push_back(normalize(vec3(-1.0f, sb, tb)));
			positions.push_back(normalize(vec3(-1.0f, sa, tb)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
			positions.push_back(normalize(vec3(1.0f, sa, tb)));
			positions.push_back(normalize(vec3(1.0f, sb, tb)));
			positions.push_back(normalize(vec3(1.0f, sb, ta)));
			positions.push_back(normalize(vec3(1.0f, sa, ta)));
			indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
			indices.push_back(i + 2); indices.push_back(i + 3); indices.push_back(i + 0);
			i += 4;
		}
	}
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
	generateSphereNormal(positions, indices);
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
bool keydown = false;
bool wireframe = true;

void update(double time)
{
	double dt = time - time0;

	if(glfwGetKey(GLFW_KEY_SPACE) && !keydown)
	{
		wireframe = !wireframe;
		keydown = true;
	}
	else if(!glfwGetKey(GLFW_KEY_SPACE))
	{
		keydown = false;
	}

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

	if(wireframe)
	{
		glUniform(program.uniforms["white"], 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_SHORT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

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

	if(!initGL("Patche Sphere", width, height, 3, 1, 24, 8, 4, false))
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

	glClearColor(0.73f, 0.73f, 0.73f, 0.73f);

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