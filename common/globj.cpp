#include "globj.h"

GLint Program::getAttribLoc(const std::string &s)
{
	std::unordered_map<std::string, GLint>::const_iterator i = attribs.find(s);
	return i != attribs.end() ? i->second : -1;
}

GLint Program::getUniformLoc(const std::string &s)
{
	std::unordered_map<std::string, GLint>::const_iterator i = uniforms.find(s);
	return i != uniforms.end() ? i->second : -1;
}

void IndexedVertexArray::addVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
{
	positions.push_back(glm::vec3(x, y, z));
	normals.push_back(glm::vec3(nx, ny, nz));
	texels.push_back(glm::vec2(u, v));
}

void IndexedVertexArray::addTriangle(GLushort i0, GLushort i1, GLushort i2)
{
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
}

void IndexedVertexArray::addQuad(GLushort i0, GLushort i1, GLushort i2, GLushort i3)
{
	addTriangle(i0, i1, i2);
	addTriangle(i2, i3, i0);
}

void IndexedVertexArray::clear()
{
	positions.clear();
	normals.clear();
	texels.clear();
}