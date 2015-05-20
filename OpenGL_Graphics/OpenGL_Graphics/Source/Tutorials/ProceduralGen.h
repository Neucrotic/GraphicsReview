#pragma once
#include "Engine/GPUParticleEmitter.h"
#include "Engine/Application.h"
#include "AntTweakBar.h"
#include "stb_image.h"
#include "FBXFile.h"
#include <vector>

class ProceduralGen : public Application
{
public:

struct Vertex
	{
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 uv;
	};

	bool Startup();
	void Shutdown();
	bool Update(double _dt);
	void Render();

private:
	
	//TweakBar properties
	TwBar* gui;

	int octaves;
	float oldPersist;
	bool PersistChanged;

	float amplitude;
	float oldAmp;
	bool ampChanged;

	float oldSeed;
	bool seedChanged;

	int gridRows;
	int gridColumns;

	//shader and model IDs
	unsigned int shaderProg;
	unsigned int untexturedProg;
	unsigned int animatedProg;
	FBXFile* fbxBunny;
	FBXFile* fbxPyro;

	//texture IDs
	unsigned int perlinTexture, grassTexture, sandTexture, rockTexture;

	unsigned int indexCount; //index used when generating/drawing the grid
	int seed; //seed used to change the procedural terrain

	//loads in a texture from file
	int LoadTexture(std::string texture);

	void GenerateTerrain(unsigned int rows);
	void GenerateGrid(unsigned int rows, unsigned int cols);
	float* GeneratePerlinData(int _dims, int _scale);

	void CreateOpenGLBuffers(FBXFile* _model);
	void CleanupOpenGLBuffers(FBXFile* _model);
};