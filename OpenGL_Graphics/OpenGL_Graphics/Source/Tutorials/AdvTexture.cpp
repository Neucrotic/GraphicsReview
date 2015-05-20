#include "AdvTexture.h"
#include "Engine/MobileCamera.h"
#include "Engine/ShaderHandler.h"
#include "Engine/MaterialHandler.h"
#include "DefaultMeshes/CubeMesh.h"
#include "Engine/Material.h"
#include "Engine/TextureLoader.h"

bool AdvTexture::Startup()
{
	MobileCamera* camera = new MobileCamera(100.0f, 0.1f);
	camera->SetInputWindow(window);
	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	//setting up quad for texture drawing
	float vertexData[] = 
	{
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0,
	};
	unsigned int indexData[] = 
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);
	glGenBuffers(1, &buffers->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &buffers->IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	textureLoader = new TextureLoader();

	//shader initialization
	shaderProg = ShaderHandler::Get()->LoadShader((string)"TexShader", "data/shaders/advTexture.vert", "data/shaders/advTexture.frag");
	
	cube = new CubeMesh(10.0f);	

	Material* pRockMaterial = MaterialHandler::Get()->CreateNewMaterial((string)"Rock");
	pRockMaterial->SetTexture(Material::DIFFUSE, textureLoader->LoadTexture((string)"Data/textures/rock_diffuse.tga"));
	pRockMaterial->SetTexture(Material::NORMAL, textureLoader->LoadTexture((string)"Data/textures/rock_normal.tga"));
	pRockMaterial->SetShader(shaderProg);
	cube->SetMaterial((string)"Rock");

	return true;
}

void AdvTexture::Shutdown()
{

}

bool AdvTexture::Update(double _dt)
{
	m_camera->Update(_dt);
	return true;
}

void AdvTexture::Render()
{
	cube->PrepareToRender(m_camera->GetTransform());
	cube->Render(m_camera, m_camera->GetTransform());
}