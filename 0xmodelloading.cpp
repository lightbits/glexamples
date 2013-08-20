#include <src/glutils.h>
#include <src/program.h>
//#include <src/mesh.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace glm;

/* see
http://www.opengl.org/wiki/Sampler_(GLSL)
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
*/

Program program0;
GLuint vsShader0, fsShader0;

struct DrawCall
{
	GLenum mode; // what kind of primitives to render
	GLsizei count; // number of elements
	GLenum type; // type of values
	GLuint start; // glDrawElements will count sequential elements beginning at this index
};

struct Mesh
{	
	std::vector<GLuint> textures;
	GLuint vbo; // vertex buffer object
	GLuint vao; // vertex array object
	GLuint ibo; // index buffer object
	std::unordered_map<int, DrawCall> drawCalls; // key: texture index
};

Mesh mesh0;

bool loadMesh(Mesh &mesh)
{
	std::vector<std::string> textureNames;
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
	std::unordered_map<int, DrawCall> drawCalls;
	int drawCallIndex = 0;

	std::string basedir = "D:/Programming/docs/mmdx/models/shiomiku";
	std::ifstream file("D:/Programming/docs/mmdx/models/shiomiku/shiomiku.txt");
	if(file.is_open())
	{
		while(file.good())
		{
			std::string line;
			std::getline(file, line);
			std::stringstream ss(line);

			std::string prefix; ss>>prefix;

			if(prefix == "v")
			{
				float x, y, z, nx, ny, nz, u, v;
				ss>>x>>y>>z>>nx>>ny>>nz>>u>>v;
				vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
				vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
				vertices.push_back(u); vertices.push_back(v);
			}
			else if(prefix == "i")
			{
				int textureIndex;
				GLushort vertexIndex;
				ss>>textureIndex>>vertexIndex;
				indices.push_back(vertexIndex);

				if(drawCalls.find(textureIndex) != drawCalls.end())
				{
					drawCalls.find(textureIndex)->second.count++;
				}
				else
				{
					DrawCall dc;
					dc.mode = GL_TRIANGLES;
					dc.count = 1;
					dc.type = GL_UNSIGNED_SHORT;
					dc.start = drawCallIndex;
					drawCalls[textureIndex] = dc;
				}

				++drawCallIndex;
			}
			else if(prefix == "t")
			{
				std::string textureName; ss>>textureName;
				textureNames.push_back(textureName);
			}
		}
	}
	else 
	{
		std::cout<<"could not open file"<<std::endl;
		return false;
	}
	file.close();

	// load textures
	std::vector<GLuint> textures;
	for(int i = 0; i < textureNames.size(); ++i)
	{
		std::string texturePath = basedir + "/" + textureNames[i];
		GLuint texture;
		if(!loadTexture(texture, texturePath, GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE))
			return false;
		std::cout<<"loaded "<<textureNames[i]<<std::endl;
		textures.push_back(texture);
	}

	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh.drawCalls = drawCalls;
	mesh.textures = textures;
	mesh.ibo = ibo;
	mesh.vao = vao;
	mesh.vbo = vbo;

	return true;
}

void renderMesh(Mesh &mesh)
{
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	glEnableVertexAttribArray(program0.attribs["position"]);
	glEnableVertexAttribArray(program0.attribs["normal"]);
	glEnableVertexAttribArray(program0.attribs["texel"]);
	glVertexAttribPointer(program0.attribs["position"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(program0.attribs["normal"], 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(program0.attribs["texel"], 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glUniform(program0.uniforms["texBaseImage"], 0); // texture unit 0 is for base image
	glUniform(program0.uniforms["texNormalMap"], 1); // texture unit 2 is for normal maps

	glActiveTexture(GL_TEXTURE0 + 0);
	for(std::unordered_map<int, DrawCall>::iterator i = mesh.drawCalls.begin(); i != mesh.drawCalls.end(); ++i)
	{
		GLuint texture = mesh.textures[i->first];
		glBindTexture(GL_TEXTURE_2D, texture);

		DrawCall dc = i->second;
		glDrawElements(dc.mode, dc.count, dc.type, reinterpret_cast<const GLvoid*>(dc.start * sizeof(GLushort)));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void deleteMesh(Mesh &mesh)
{
	for(int i = 0; i < mesh.textures.size(); ++i)
		glDeleteTextures(1, &mesh.textures[i]);
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
}

void initShaders()
{
	std::string normalmapVsSrc, normalmapFsSrc;
	if(!readFile("data/diffuse.vs", normalmapVsSrc) ||
		!readFile("data/diffuse.fs", normalmapFsSrc))
		std::cerr<<"Failure reading shader data"<<std::endl;

	vsShader0 = getShader(GL_VERTEX_SHADER, normalmapVsSrc);
	fsShader0 = getShader(GL_FRAGMENT_SHADER, normalmapFsSrc);
}

void initPrograms()
{
	program0.handle = getProgram(vsShader0, fsShader0);

	program0.attribs["position"] = glGetAttribLocation(program0.handle, "position");
	program0.attribs["normal"] = glGetAttribLocation(program0.handle, "normal");
	program0.attribs["color"] = glGetAttribLocation(program0.handle, "color");
	program0.attribs["texel"] = glGetAttribLocation(program0.handle, "texel");

	program0.uniforms["model"] = glGetUniformLocation(program0.handle, "model");
	program0.uniforms["view"] = glGetUniformLocation(program0.handle, "view");
	program0.uniforms["projection"] = glGetUniformLocation(program0.handle, "projection");

	program0.uniforms["lightPos"] = glGetUniformLocation(program0.handle, "lightPos");
	program0.uniforms["lightColor"] = glGetUniformLocation(program0.handle, "lightColor");
	program0.uniforms["ambient"] = glGetUniformLocation(program0.handle, "ambient");

	program0.uniforms["texBaseImage"] = glGetUniformLocation(program0.handle, "texBaseImage");
	program0.uniforms["texNormalMap"] = glGetUniformLocation(program0.handle, "texNormalMap");
}

const int windowWidth = 640;
const int windowHeight = 480;

vec3 lightPos = vec3(0.0f, 0.0f, 3.0f);
vec4 lightColor = vec4(0.9f, 0.95f, 1.0f, 1.0f);
vec4 ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
mat4 view = translate(0, 0, -5.0f) * rotateX(-0.45f) * rotateY(1.54f);
vec3 transl = vec3(0.0f, 0.0f, -5.0f);
mat4 model = mat4(1.0f);
mat4 projection = glm::perspective(45.0f, windowWidth / float(windowHeight), 0.1f, 15.0f);

void update(float dt)
{
	const float moveSpeed = 10.0f; // units per second

	if(glfwGetKey('A')) lightPos.x -= moveSpeed * dt;
	else if(glfwGetKey('D')) lightPos.x += moveSpeed * dt;
	if(glfwGetKey('W')) lightPos.z -= moveSpeed * dt;
	else if(glfwGetKey('S')) lightPos.z += moveSpeed * dt;
	if(glfwGetKey('Q')) lightPos.y -= moveSpeed * dt;
	else if(glfwGetKey('E')) lightPos.y += moveSpeed * dt;

	if(glfwGetKey(GLFW_KEY_LEFT)) transl.x -= moveSpeed * dt;
	else if(glfwGetKey(GLFW_KEY_RIGHT)) transl.x += moveSpeed * dt;
	if(glfwGetKey(GLFW_KEY_UP)) transl.y -= moveSpeed * dt;
	else if(glfwGetKey(GLFW_KEY_DOWN)) transl.y += moveSpeed * dt;
	if(glfwGetKey('Z')) transl.z -= moveSpeed * dt;
	else if(glfwGetKey('X')) transl.z += moveSpeed * dt;

	view = translate(transl.x, transl.y, transl.z) * rotateX(-0.45f) * rotateY(3.14f);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program0.handle);
	glUniform(program0.uniforms["model"], model);
	glUniform(program0.uniforms["view"], view);
	glUniform(program0.uniforms["projection"], projection);

	glUniform(program0.uniforms["lightPos"], lightPos);
	glUniform(program0.uniforms["lightColor"], lightColor);
	glUniform(program0.uniforms["ambient"], ambient);

	renderMesh(mesh0);
}

int main()
{
	if(!createContext("glvoxel", windowWidth, windowHeight, 24, 8, 4, false))
		return EXIT_FAILURE;

	if(!loadMesh(mesh0))
		return EXIT_FAILURE;

	initShaders();
	initPrograms();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_2D);

	const double targetRenderTime = 1.0 / 60.0;
	double renderTime = 0.0;
	double dt = 0.0;
	while(glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
	{
		double frameStart = glfwGetTime();

		glfwPollEvents();
		update(dt);

		double renderStart = glfwGetTime();
		render();
		glfwSwapBuffers();
		renderTime = glfwGetTime() - renderStart;

		// shitty framerate control
		if(renderTime < targetRenderTime)
			glfwSleep(targetRenderTime - renderTime);

		dt = glfwGetTime() - frameStart;
	}

	deleteMesh(mesh0);
	glDeleteShader(vsShader0);
	glDeleteShader(fsShader0);
	glDeleteProgram(program0.handle);
	destroyContext();
}