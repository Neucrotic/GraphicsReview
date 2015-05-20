#pragma once
#include "Engine/Application.h"

class TextureLoader;
class CubeMesh;
class ObjectMesh;

class AdvTexture : public Application
{
public:

	bool Startup();
	void Shutdown();
	bool Update(double _dt);
	void Render();

private:

	TextureLoader* textureLoader;
	CubeMesh* cube;
	ObjectMesh* obj;

	unsigned int shaderProg;

};