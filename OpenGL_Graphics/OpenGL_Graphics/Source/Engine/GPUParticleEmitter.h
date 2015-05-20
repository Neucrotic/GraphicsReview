#pragma once
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ShaderLoader;

class GPUParticleEmitter
{
	struct GPUParticle
	{
		GPUParticle() : lifetime(1), lifespan(0) {}

		glm::vec3 position;
		glm::vec3 velocity;
		float lifetime;
		float lifespan;
	};

protected:

	ShaderLoader* shaderLoader;

	GPUParticle* particles;
	GPUParticle* tempArray;
	unsigned int maxParticles;

	glm::vec3 position;

	float lifespanMin;
	float lifespanMax;

	float velocityMin;
	float velocityMax;

	float startSize;
	float endSize;

	glm::vec4 startColour;
	glm::vec4 endColour;

	unsigned int activeBuffer;
	unsigned int VAO[2];
	unsigned int VBO[2];

	unsigned int drawShader;
	unsigned int updateShader;

	float lastDrawTime;

	void CreateBuffers();
	void CreateDrawShader();
	
public:

	GPUParticleEmitter();
	~GPUParticleEmitter();

	void Init(unsigned int _maxParticles,
		float _lifespanMin, float _lifespanMax,
		float _velocityMin, float _velocityMax,
		float _startSize, float _endSize,
		const glm::vec4& _startColour,
		const glm::vec4& _endColour);

	void CreateUpdateShader(const char* _updateShader);
	void Draw(float _time, const glm::mat4& _cameraTransform, const glm::mat4& _projectionView);
	void SetOrigin(glm::vec3 _origin);
};