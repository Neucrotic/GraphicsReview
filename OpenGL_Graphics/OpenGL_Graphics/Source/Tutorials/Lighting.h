#pragma once
#include "Engine/Application.h"
#include "FBXFile.h"

class ObjectMesh;

class Lighting : public Application
{
public:

	bool Startup();
	void Shutdown();
	bool Update(double _dt);
	void Render();

private:

	FBXFile* fbxModel;
	unsigned int shaderProg;

	void CreateShaders();

	void CreateOpenGLBuffers(FBXFile* _model);
	void CleanupOpenGLBuffers(FBXFile* _model);

};