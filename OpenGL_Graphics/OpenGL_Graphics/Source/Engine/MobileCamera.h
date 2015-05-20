#pragma once
#include "Engine/Camera.h"
#include <GLFW\glfw3.h>

class MobileCamera : public Camera
{
public:

	MobileCamera() : MobileCamera(10.0f, 5.0f)
	{
		viewButtonClicked = false;
	}
	MobileCamera(float _flySpeed, float _rotSpeed) : Camera(), moveSpeed(_flySpeed), rotationSpeed(_rotSpeed)
	{

	}
	~MobileCamera() {};

	virtual void Update(double _dt);

	inline void SetMovespeed(float _moveSpeed) { moveSpeed = _moveSpeed; }
	inline float GetMovespeed() const { return moveSpeed; }

	inline float GetRotationSpeed() const { return rotationSpeed; }
	inline void SetRotationSpeed(float _value) { rotationSpeed = _value; }

	inline void SetInputWindow(GLFWwindow* _window) { window = _window; } //implement input manager instead

protected:

	void HandleKeyboardInput(double _dt);
	void HandleMouseInput(double _dt);

	void CalculateRotation(double _dt, double _xOffset, double _yOffset);

	GLFWwindow* window;
	float moveSpeed;
	float rotationSpeed;
	bool viewButtonClicked;
	double cursorX;
	double cursorY;

};