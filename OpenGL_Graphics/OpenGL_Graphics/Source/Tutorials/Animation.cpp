#include "Animation.h"
#include "Engine\MobileCamera.h"
#include "Engine\ShaderHandler.h"

bool Animation::Startup()
{
	MobileCamera* camera = new MobileCamera(1000.0f, 0.1f);
	camera->SetInputWindow(window);
	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	fbxModel = new FBXFile();
	fbxModel->load("Data/models/Pyro/pyro.fbx");
	fbxModel->initialiseOpenGLTextures();
	CreateOpenGLBuffers(fbxModel);

	shaderProg = ShaderHandler::Get()->LoadShader((string)"ModelShader", "Data/shaders/lighting.vert", "Data/shaders/lighting.frag");

	return true;
}

void Animation::Shutdown()
{
	CleanUpOpenGLBuffers(fbxModel);
}

bool Animation::Update(double _dt)
{
	m_camera->Update(_dt);

	FBXSkeleton* skeleton = fbxModel->getSkeletonByIndex(0);
	FBXAnimation* animation = fbxModel->getAnimationByIndex(0);

	float timer = animation->totalTime();

	skeleton->evaluate(animation, glfwGetTime());

	for (uint boneIndex = 0; boneIndex < skeleton->m_boneCount; ++boneIndex)
	{
		skeleton->m_nodes[boneIndex]->updateGlobalTransform();
	}

	return true;
}

void Animation::Render()
{
	glUseProgram(shaderProg);
	int location = glGetUniformLocation(shaderProg, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	location = glGetUniformLocation(shaderProg, "LightDir");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 0.5f, 0)));
	location = glGetUniformLocation(shaderProg, "LightColour");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	location = glGetUniformLocation(shaderProg, "CameraPos");
	glUniform3fv(location, 1, glm::value_ptr(m_camera->GetPosition()));
	location = glGetUniformLocation(shaderProg, "SpecPow");
	glUniform1f(location, 28);

	//getting the skeleton and animation
	FBXSkeleton* skeleton = fbxModel->getSkeletonByIndex(0);
	skeleton->updateBones();
	location = glGetUniformLocation(shaderProg, "bones");
	glUniformMatrix4fv(location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	//bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < fbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxModel->getMeshByIndex(i);

		uint* glData = (uint*)mesh->m_userData;

		//mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		glUniform1i(glGetUniformLocation(shaderProg, "diffuse"), 0);

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (uint)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void Animation::CreateOpenGLBuffers(FBXFile* _model)
{
	for (unsigned int i = 0; i < _model->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _model->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); //normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2); //uv
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glEnableVertexAttribArray(3); //weights
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);

		glEnableVertexAttribArray(4); //indices
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Animation::CleanUpOpenGLBuffers(FBXFile* _model)
{
	for (unsigned int i = 0; i < _model->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = _model->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}