/* 
OpenGL examples - Normal Mapping

Applies a normal map texture to perturb the surface normal, and give the illusion of detailed surfaces
http://www.opengl.org/wiki/Sampler_(GLSL)
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
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
GLuint baseImage;
GLuint normalMap;
GLuint vbo, vao, ibo;
IndexedVertexArray iva;

mat4 model = mat4(1.0f);
//mat4 view = translate(0.0f, 0.0f, -3.0f) * rotateX(-0.59f) * rotateY(0.35f);
mat4 view = translate(0.0f, 0.0f, -3.0f) * rotateX(-0.6f);
mat4 projection = glm::perspective(45.0f, 640.0f / 480.0f, 0.1f, 10.0f);

vec3 lightPos = vec3(0.0f, 0.0f, 3.0f);
vec4 lightColor = vec4(0.9f, 0.95f, 1.0f, 1.0f);
vec4 ambient = vec4(0.2f, 0.2f, 0.38f, 1.0f);

/* Computes an orthogonal tangent space basis from the given vertex data. */
void computeTangentBasis(
std::vector<vec3> &positions, std::vector<vec3> &normals, std::vector<vec2> &texels,
std::vector<vec3> &tangents, std::vector<vec3> &bitangents)
{
	tangents.clear();
	bitangents.clear();
	// Go through each triangle and calculate the tangent space vectors
	for(int i = 0; i < positions.size() - 2; i += 3)
	{
		vec3 &v0 = positions[i + 0];
		vec3 &v1 = positions[i + 1];
		vec3 &v2 = positions[i + 2];

		vec2 &uv0 = texels[i + 0];
		vec2 &uv1 = texels[i + 1];
		vec2 &uv2 = texels[i + 2];

		// Edges of triangle
		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		// UV delta
		vec2 deltaUv1 = uv1 - uv0;
		vec2 deltaUv2 = uv2 - uv0;

		// Solve for tangent vectors
		float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
		vec3 tangent = (deltaPos1 * deltaUv2.y - deltaPos2 * deltaUv1.y) * r;
		vec3 bitangent = (deltaPos2 * deltaUv1.x - deltaPos1 * deltaUv2.x) * r;

		// Orthogonalize basis (by "pushing" the tangent vector to make it orthogonal
		// to the surface normal) (DOES THIS ACTUALLY WORK?)
		vec3 t0 = normalize(tangent - normals[i+0] * dot(tangent, normals[i+0]));
		vec3 t1 = normalize(tangent - normals[i+1] * dot(tangent, normals[i+1]));
		vec3 t2 = normalize(tangent - normals[i+2] * dot(tangent, normals[i+2]));

		// Store the tangent/bitangents for each vertex
		tangents.push_back(t0);
		tangents.push_back(t1);
		tangents.push_back(t2);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}

bool loadTextures()
{
	// create 4x4 checkerboard rgba texture
	int width = 4;
	int height = 4;
	std::vector<GLubyte> pixels(width * height * 4);
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

	baseImage = createTexture2d(width, height, &pixels[0], GL_UNSIGNED_BYTE, GL_RGBA);
	glBindTexture(GL_TEXTURE_2D, baseImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(!loadTexture(normalMap, "data/normal.png", GL_TEXTURE_2D, 
		GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE))
		return false;

	return true;
}

void initProgram()
{
	std::string vsSrc, fsSrc;
	if(!readFile("data/normalmap.vs", vsSrc) ||
		!readFile("data/normalmap.fs", fsSrc))
		std::cerr<<"Failure reading shader data"<<std::endl;

	vsShader = getShader(GL_VERTEX_SHADER, vsSrc);
	fsShader = getShader(GL_FRAGMENT_SHADER, fsSrc);
	program.handle = getProgram(vsShader, fsShader);

	program.attribs["position"] = glGetAttribLocation(program.handle, "position");
	program.attribs["normal"] = glGetAttribLocation(program.handle, "normal");
	program.attribs["texel"] = glGetAttribLocation(program.handle, "texel");
	program.attribs["tangent"] = glGetAttribLocation(program.handle, "tangent");
	program.attribs["bitangent"] = glGetAttribLocation(program.handle, "bitangent");

	program.uniforms["model"] = glGetUniformLocation(program.handle, "model");
	program.uniforms["view"] = glGetUniformLocation(program.handle, "view");
	program.uniforms["projection"] = glGetUniformLocation(program.handle, "projection");
	program.uniforms["lightPos"] = glGetUniformLocation(program.handle, "lightPos");
	program.uniforms["lightColor"] = glGetUniformLocation(program.handle, "lightColor");
	program.uniforms["ambient"] = glGetUniformLocation(program.handle, "ambient");
	program.uniforms["texBaseImage"] = glGetUniformLocation(program.handle, "texBaseImage");
	program.uniforms["texNormalMap"] = glGetUniformLocation(program.handle, "texNormalMap");
}

void initBuffers()
{
	iva.clear();
	iva.addVertex(-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	iva.addVertex(-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	iva.addVertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	iva.addTriangle(0, 1, 2);

	iva.addVertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	iva.addVertex( 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	iva.addVertex(-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	iva.addTriangle(3, 4, 5);

	iva.addVertex(-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	iva.addVertex(-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	iva.addVertex( 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	iva.addTriangle(6, 7, 8);

	iva.addVertex( 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	iva.addVertex( 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	iva.addVertex(-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	iva.addTriangle(9, 10, 11);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Compute tangent basis vectors for each vertex
	std::vector<vec3> tangents;
	std::vector<vec3> bitangents;
	computeTangentBasis(iva.positions, iva.normals, iva.texels, tangents, bitangents);

	// Compute byte sizes/offsets
	GLsizeiptr b0 = iva.positions.size() * sizeof(vec3);
	GLsizeiptr b1 = iva.normals.size() * sizeof(vec3);
	GLsizeiptr b2 = iva.texels.size() * sizeof(vec2);
	GLsizeiptr b3 = tangents.size() * sizeof(vec3);
	GLsizeiptr b4 = bitangents.size() * sizeof(vec3);

	// Create vertex buffer object to hold the vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, b0 + b1 + b2 + b3 + b4, NULL, GL_STATIC_DRAW);

	// Upload vertex data in chunks
	glBufferSubData(GL_ARRAY_BUFFER, 0,					b0,	&iva.positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, b0,				b1,	&iva.normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, b0 + b1,			b2,	&iva.texels[0]);
	glBufferSubData(GL_ARRAY_BUFFER, b0 + b1 + b2,		b3,	&tangents[0]);
	glBufferSubData(GL_ARRAY_BUFFER, b0 + b1 + b2 + b3, b4, &bitangents[0]);

	// Enable and specify vertex format
	glEnableVertexAttribArray(program.getAttribLoc("position"));
	glEnableVertexAttribArray(program.getAttribLoc("normal"));
	glEnableVertexAttribArray(program.getAttribLoc("texel"));
	glEnableVertexAttribArray(program.getAttribLoc("tangent"));
	glEnableVertexAttribArray(program.getAttribLoc("bitangent"));
	glVertexAttribPointer(program.getAttribLoc("position"),	3,	GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribPointer(program.getAttribLoc("normal"),	3,	GL_FLOAT, GL_FALSE, 0, (void*)(b0));
	glVertexAttribPointer(program.getAttribLoc("texel"),	2,	GL_FLOAT, GL_FALSE, 0, (void*)(b0 + b1));
	glVertexAttribPointer(program.getAttribLoc("tangent"),	3,	GL_FLOAT, GL_FALSE, 0, (void*)(b0 + b1 + b2));
	glVertexAttribPointer(program.getAttribLoc("bitangent"), 3,	GL_FLOAT, GL_FALSE, 0, (void*)(b0 + b1 + b2 + b3));

	// Create index buffer object to hold the index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iva.indices.size() * sizeof(GLushort), &iva.indices[0], GL_STATIC_DRAW);

	// "Unbind" vao and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

double time0 = 0.0;
void update(double time)
{
	double dt = time - time0;

	model = rotateY(sinf(time * 0.5f));
	//lightPos.x = sinf(time * 2.0f);
	//lightPos.z = cosf(time * 3.0f);

	if(glfwGetKey('A'))
		lightPos.x -= 2.0f * dt;
	else if(glfwGetKey('D'))
		lightPos.x += 2.0f * dt;

	if(glfwGetKey('W'))
		lightPos.z -= 2.0f * dt;
	else if(glfwGetKey('S'))
		lightPos.z += 2.0f * dt;

	if(glfwGetKey('Z'))
		lightPos.y -= 2.0f * dt;
	else if(glfwGetKey('X'))
		lightPos.y += 2.0f * dt;

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
	glUniform(program.uniforms["texNormalMap"], 1); // texture unit 1 is for base image

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, baseImage);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glDrawElements(GL_TRIANGLES, iva.indices.size(), GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	// Draw light position
	glPointSize(10.0f);
	glBegin(GL_POINTS);
		vec4 p = projection * view * vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f);
		glColor3f(1.0f, 0.2f, 0.2f);
		glVertex3f(p.x / p.w, p.y / p.w, p.z / p.w);
	glEnd();

	glfwSwapBuffers();
}

int main()
{
	int width = 640;
	int height = 480;

	if(!initGL("Normalmapping", width, height, 3, 1, 24, 8, 4, false))
		exit(EXIT_FAILURE);

	loadTextures();
	initProgram();
	initBuffers();

	glClearColor(0.55f, 0.59f, 0.95f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f); // depth values are clamped to [0, 1] anyway, so this will fully utilize the depth buffer
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

		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::cerr<<getErrorMessage(error)<<std::endl;
			std::cin.get();
			glfwCloseWindow();
		}
		
		// let cpu get some sleep
		glfwSleep(0.013);
	}

	glDeleteTextures(1, &baseImage);
	glDeleteTextures(1, &normalMap);
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