/*
OpenGL examples - Camera

a virtual camera that can be positioned and oriented in the scene.
the worldToView transformation matrix can be obtained through either
	getViewMatrix(),
which returns the view as seen from the camera's position and direction,
or
	getViewMatrixFocus(),
which returns the view as seen from a camera directed towards _focus_,
at a distance of _radius_. The orientation around the focus point is
determined by the horizontal and vertical angles, theta and phi.
*/

#ifndef CAMERA_H
#define CAMERA_H
#include <src/glutils.h>

class Camera
{
public:
	Camera();

	void rotateLeft(float t);
	void rotateRight(float t);
	void rotateUp(float t);
	void rotateDown(float t);

	void setHorizontalAngle(float t);
	void setVerticalAngle(float t);
	void setPosition(const glm::vec3 &p);

	void updateVectors();

	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrixFocus(float radius, const glm::vec3 &focus);
private:
	float theta;
	float phi;
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 forward;
};

#endif