#include "common/glutils.h"
using namespace glm;

Camera::Camera() : theta(0), phi(0), position(0, 0, -1)
{
	updateVectors();
}

void Camera::rotateLeft(float t) { theta -= t; if(t < 0.0f) t += TWO_PI; }
void Camera::rotateRight(float t) { theta += t; if(t > TWO_PI) t -= TWO_PI; }
void Camera::rotateUp(float t) { if(phi < PI) phi += t; }
void Camera::rotateDown(float t) { if(phi > -PI) phi -= t; }

void Camera::setHorizontalAngle(float t) { theta = mod(t, TWO_PI); }
void Camera::setVerticalAngle(float t) { phi = mod(abs(t), PI) * (t < 0 ? -1 : 1); }
void Camera::setPosition(const glm::vec3 &p) { position = p; }

void Camera::updateVectors()
{
	float sintheta = sinf(theta);
	float costheta = cosf(theta);
	float sinphi = sinf(phi);
	float cosphi = cosf(phi);
	forward = vec3(cosphi * sintheta, sinphi, -cosphi * costheta);
	right = vec3(costheta, 0.0f, sintheta);
	up = cross(right, forward);
}

mat4 Camera::getViewMatrix() { return glm::lookAt(position, position + forward, up); }
mat4 Camera::getViewMatrixFocus(float radius, const vec3 &focus) { return glm::lookAt(focus + radius * forward, focus, up); }