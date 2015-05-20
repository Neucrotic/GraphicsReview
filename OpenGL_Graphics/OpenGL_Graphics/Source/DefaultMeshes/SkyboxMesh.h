#pragma once
#include "Engine/Renderable.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

using namespace std;

class SkyboxMesh : public Renderable
{
public: 

	SkyboxMesh(string _folderName, string _extension) : folderName(_folderName), extension(_extension) { Create(); }

	void Create();
	void Render(Camera* _camera, glm::mat4 _transform);

	void CreateVertices();
	void CreateMaterial();

private:

	string folderName;
	string extension;

};