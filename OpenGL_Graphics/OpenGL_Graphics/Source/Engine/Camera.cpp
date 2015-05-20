#include "Camera.h"
#include <glm/ext.hpp>

Camera::Camera()
	: Camera(glm::mat4(1)) //initialising unit matrix
{

}

Camera::Camera(glm::vec3 _position)
	: Camera() //calling base constructor
{
	worldTransform[3] = glm::vec4(_position, 1);
	perspectiveSet = false;
}

Camera::Camera(glm::mat4 _transform)
	: worldTransform(_transform) //initialisng world transform inline
{
	UpdateProjectionViewTransform();
}

void Camera::SetTransform(glm::mat4 _transform)
{
	worldTransform = _transform;
	UpdateProjectionViewTransform();
}

const glm::mat4 Camera::GetTransform() const
{
	return worldTransform;
}

void Camera::SetPosition(glm::vec3 _position)
{
	worldTransform[3] = glm::vec4(_position, 1);
	UpdateProjectionViewTransform();
}

//setup functions
void Camera::LookAt(glm::vec3 _lookAt, glm::vec3 _up)
{
	glm::vec4 location = worldTransform[3]; //current position data saved because not passed in
	worldTransform = glm::inverse(glm::lookAt(location.xyz(), _lookAt, _up));

	UpdateProjectionViewTransform();
}

void Camera::LookAt(glm::vec3 _position, glm::vec3 _lookAt, glm::vec3 _up)
{
	worldTransform = glm::inverse(glm::lookAt(_position, _lookAt, _up)); //position data passed in

	UpdateProjectionViewTransform();
}

void Camera::SetupPerspective(float _fov, float _aspectRatio, float _near /* = 0.01f*/, float _far /* = 1000.0f*/)
{
	projectionTransform = glm::perspective(_fov, _aspectRatio, _near, _far);
	perspectiveSet = true;

	UpdateProjectionViewTransform();
}

void Camera::UpdateProjectionViewTransform()
{
	viewTransform = glm::inverse(worldTransform);
	projectionViewTransform = projectionTransform * viewTransform;
}