#include "MobileCamera.h"
#include <glm/ext.hpp>

#include <iostream>

void MobileCamera::Update(double _dt)
{
	HandleKeyboardInput(_dt);
	HandleMouseInput(_dt);
}

void MobileCamera::HandleKeyboardInput(double _dt)
{
	//save copies of the cameras forward/up/right
	glm::mat4 transform = GetTransform();

	glm::vec3 forward = transform[2].xyz;
	glm::vec3 up = transform[1].xyz;
	glm::vec3 right = transform[0].xyz;

	glm::vec3 moveDirection(0.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDirection -= forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDirection += forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDirection -= right;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDirection += right;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDirection += glm::vec3(0.0f, -1.0f, 0.0f);
	}

	float length = glm::length(moveDirection);
	if (length > 0.01f)
	{
		moveDirection = ((float)_dt * moveSpeed) * glm::normalize(moveDirection);
		SetPosition(GetPosition() + moveDirection);
	}
}

void MobileCamera::HandleMouseInput(double _dt)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (viewButtonClicked == false)
		{
			int width;
			int height;

			glfwGetFramebufferSize(window, &width, &height);

			cursorX = width / 2.0;
			cursorY = height / 2.0;

			glfwSetCursorPos(window, width / 2, height / 2);

			viewButtonClicked = true;
		}
		else
		{
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			double xOffset = mouseX - cursorX;
			double yOffset = mouseY - cursorY;

			CalculateRotation(_dt, xOffset, yOffset);
		}

		int width;
		int height;
		glfwGetFramebufferSize(window, &width, &height);
		glfwSetCursorPos(window, width / 2, height / 2);
	}
	else
	{
		viewButtonClicked = false;
	}
}

void MobileCamera::CalculateRotation(double _dt, double _xOffset, double _yOffset) //implement quaternions
{
	if (_xOffset != 0.0)
	{
		//glm::rotate rotation speec X deltaTime X inverse xOffset, against world up
		glm::mat4 rotX = glm::rotate((float)(rotationSpeed * _dt * -_xOffset), glm::vec3(0, 1, 0));

		SetTransform(GetTransform() * rotX);
	}

	if (_yOffset != 0.0)
	{
		//glm::rotate rotation speec X deltaTime X inverse yOffset, against world forward
		glm::mat4 rotY = glm::rotate((float)(rotationSpeed * _dt * -_yOffset), glm::vec3(1, 0, 0));

		SetTransform(GetTransform() * rotY);
	}

	//Clean up to avoid skewed roations
	glm::mat4 oldTransform = GetTransform();

	glm::mat4 newTransform;

	glm::vec3 worldUp = glm::vec3(0, 1, 0);

	//Right
	newTransform[0] = glm::normalize(glm::vec4(glm::cross(worldUp, oldTransform[2].xyz()), 0));
	//Up
	newTransform[1] = glm::normalize(glm::vec4(glm::cross(oldTransform[2].xyz(), newTransform[0].xyz()), 0));
	//Forward
	newTransform[2] = glm::normalize(oldTransform[2]);

	//Position
	newTransform[3] = oldTransform[3];

	SetTransform(newTransform);
}