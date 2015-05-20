#include "ParticleSystem.h"
#include "MobileCamera.h"
#include "GPUParticleEmitter.h"
#include <Gizmos_fix.h>

bool ParticleSystem::Startup()
{
	MobileCamera* camera = new MobileCamera();
	camera->SetInputWindow(window);
	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	//loop through array and initiliaze emitters
	grassEmitter = new GPUParticleEmitter();
	grassEmitter->Init(3000, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, glm::vec4(0, 1, 0, 1), glm::vec4(0, 0.75, 0, 1));
	grassEmitter->CreateUpdateShader("Data/shaders/GPUparticleUpdateGrass.vert");
	grassEmitter->SetOrigin(glm::vec3(-10, 0, -10));

	rainEmitter = new GPUParticleEmitter();
	rainEmitter->Init(50, 1.0f, 1.5f, -800, -1000, 1, 0.5, glm::vec4(0, 0, 0.5f, 1), glm::vec4(0, 0, 1, 1));
	rainEmitter->CreateUpdateShader("Data/shaders/GPUparticleUpdateRain.vert");
	rainEmitter->SetOrigin(glm::vec3(-10, 10, -10));

	cloudEmitter = new GPUParticleEmitter();
	cloudEmitter->Init(2000, 0.5f, 1.0f, 0.25f, 0.5f, 1.0f, 0.5f, glm::vec4(1, 1, 1, 1), glm::vec4(0.5f, 0.5f, 0.5f, 0));
	cloudEmitter->CreateUpdateShader("Data/shaders/GPUparticleUpdateCloud.vert");
	cloudEmitter->SetOrigin(glm::vec3(-10, 10, -10));
	
	return true;
}

bool ParticleSystem::Update(double _dt)
{
	m_camera->Update(_dt);

	return true;
}

void ParticleSystem::Render()
{
	//drawing a grid
	Gizmos::addTransform(glm::mat4(1));

	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
			glm::vec3(-10 + i, 0, -10),
			i == 10 ? white : black);

		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
			glm::vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	//USES GPU
	grassEmitter->Draw((float)glfwGetTime(), m_camera->GetTransform(), m_camera->GetProjectionView());
	rainEmitter->Draw((float)glfwGetTime(), m_camera->GetTransform(), m_camera->GetProjectionView());
	cloudEmitter->Draw((float)glfwGetTime(), m_camera->GetTransform(), m_camera->GetProjectionView());
}

void ParticleSystem::Shutdown()
{

}