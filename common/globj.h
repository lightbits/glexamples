/*
OpenGL examples - GL Objects
*/

#ifndef GL_OBJ_H
#define GL_OBJ_H
#include "glutils.h"
#include <unordered_map>
#include <vector>

struct Program
{
	GLuint handle;
	std::unordered_map<std::string, GLint> uniforms;
	std::unordered_map<std::string, GLint> attribs;

	GLint getAttribLoc(const std::string &s);
	GLint getUniformLoc(const std::string &s);
};

class IndexedVertexArray
{
public:
	std::vector<GLushort> indices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texels;

	void clear();
	void addVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v);
	void addTriangle(GLushort i0, GLushort i1, GLushort i2);
	void addQuad(GLushort i0, GLushort i1, GLushort i2, GLushort i3);
	GLushort getLastVertexIndex() const { return positions.size() - 1; }
};

#endif