#pragma once
#include <glm\glm.hpp>

class Camera
{
public:

	Camera();
	Camera(glm::vec3 _postion);
	Camera(glm::mat4 _transform);

	virtual void Update(double _dt) = 0;


	void SetTransform(glm::mat4 _transform);
	const glm::mat4 GetTransform() const;

	void SetPosition(glm::vec3 _position);
	inline glm::vec3 GetPosition() const { return worldTransform[3].xyz; }

	inline const glm::mat4& GetProjection() const { return projectionTransform; }
	inline const glm::mat4& GetView() const { return viewTransform; }
	inline const glm::mat4& GetProjectionView() const { return projectionViewTransform; }

	inline bool GetPerspectiveSet() const { return perspectiveSet; }


	//setup functions
	void LookAt(glm::vec3 _lookAt, glm::vec3 _up);
	void LookAt(glm::vec3 _position, glm::vec3 _lookAt, glm::vec3 _up);

	void SetupPerspective(float _fov, float _aspectRatio, float _near = 0.1f, float _far = 1000.0f);

protected:

	//called after any changes to worldTransform
	void UpdateProjectionViewTransform();

	glm::mat4 viewTransform;
	glm::mat4 projectionTransform;
	glm::mat4 projectionViewTransform;

private: //not inherited, unique to each camera

	//this objects transform in world space
	glm::mat4 worldTransform;
	
	//flag for communicating whether the camera perspective is set up
	bool perspectiveSet;
};