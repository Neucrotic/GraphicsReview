#include "SkyboxMesh.h"
#include "Engine/MaterialHandler.h"
#include "Engine/ShaderHandler.h"
#include "Engine/Material.h"
#include "Engine/TextureLoader.h"

void SkyboxMesh::Create()
{
	CreateVertices();

	CreateMaterial();
}

void SkyboxMesh::Render(Camera* _camera, glm::mat4 _transform)
{
	if (!PrepareToRender(_transform))
		return;

	glDepthMask(GL_FALSE);
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

void SkyboxMesh::CreateVertices()
{
	numberOfIndices = 36;
	numberOfVerts = 36;

	GLfloat skyboxVertices[] = 
	{
		//positions

		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	//generting and binding buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
}

void SkyboxMesh::CreateMaterial()
{
	Material* material = new Material();

	unsigned int cubeMap = MaterialHandler::Get()->GetTextureLoader()->LoadCubeMap(folderName, extension, TextureLoader::POSITION);

	material->SetTexture(Material::CUBE, cubeMap);
	material->SetShader(ShaderHandler::Get()->LoadShader((string)"Skybox", "Shaders/skybox.vert", "Shaders/skybox.frag"));
	SetMaterial(material);
}