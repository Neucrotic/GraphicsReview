#pragma once
#include "Application.h"

class ParticleEmitter;
class GPUParticleEmitter;

class ParticleSystem : public Application
{
protected:
	bool Startup();
	void Shutdown();

	bool Update(double dt);
	void Render();

	ParticleEmitter* emitter;
	GPUParticleEmitter* grassEmitter;
	GPUParticleEmitter* rainEmitter;
	GPUParticleEmitter* cloudEmitter;
};