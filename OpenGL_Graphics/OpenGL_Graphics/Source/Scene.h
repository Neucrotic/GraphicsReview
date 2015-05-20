#pragma once
#include "Engine/Application.h"
#include "AntTweakBar.h"
#include "Engine\GPUParticleEmitter.h"
#include <vector>

struct Vertex
{
	glm::vec4 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Scene : public Application
{
public:

	bool Startup();
	void Shutdown();
	bool Update(double _dt);
	void Render();

private:

	//tweak bar properties
	TwBar* tweakBar;
	float editAmplitude;
	float editScale;
	bool barChanged;
	float oldAmp;
	float oldPers;

	//properties used for Procedural Generation
	unsigned int terrainShader;
	unsigned int planeShader;
	int gridRows;
	int gridColumns;
	float perlinSeed;
	unsigned int perlinTexture;
	unsigned int planeTexture;
	unsigned int* indices;
	std::vector<Vertex> vertices;
		
	float* GeneratePerlinData(int _dims, int _scale, float _amplitude);
	void GenerateTerrain(int _dimensions, int _terrScale, float _terrAmplitude);
	void CreatePlane(int _dimensions);
	void CreateOpenGLBuffers(std::vector<Vertex> _verts);
	void GenerateNormals();
	void DrawNormals();

	//properties used for Particles
	GPUParticleEmitter* cloudEmitter;
	GPUParticleEmitter* rainEmitter;

	//properties used for FBX


};