#pragma once
#include "Engine\Application.h"
#include "FBXFile.h"

typedef unsigned int uint;

class Animation : public Application
{
public:

	bool Startup();
	void Shutdown();
	bool Update(double _dt);
	void Render();

private:

	FBXFile* fbxModel;
	uint shaderProg;

	void CreateOpenGLBuffers(FBXFile* _model);
	void CleanUpOpenGLBuffers(FBXFile* _model);

};