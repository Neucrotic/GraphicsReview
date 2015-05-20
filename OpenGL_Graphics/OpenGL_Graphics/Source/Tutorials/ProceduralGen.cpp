#include "ProceduralGen.h"
#include "Engine\MobileCamera.h"
#include "Engine\ShaderHandler.h"

#pragma region TweakBar Overrides
void OnMouseButton(GLFWwindow*, int b, int a, int m)
{
	TwEventMouseButtonGLFW(b, a);
}
void OnMousePosition(GLFWwindow*, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}
void OnMouseScroll(GLFWwindow*, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}
void OnKey(GLFWwindow*, int k, int s, int a, int m)
{
	TwEventKeyGLFW(k, a);
}
void OnChar(GLFWwindow*, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}
void OnWindowResize(GLFWwindow*, int w, int h)
{
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}
#pragma endregion

bool ProceduralGen::Startup()
{
	MobileCamera* camera = new MobileCamera(1000.0f, 0.1f);
	camera->SetInputWindow(window);
	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	GenerateGrid(200, 200);
	GenerateTerrain(200);

	shaderProg = ShaderHandler::Get()->LoadShader((string)"GridShader", "Data/shaders/litTerrain.vert", "Data/shaders/litTerrain.frag", "Data/shaders/litTerrain.geom");
	untexturedProg = ShaderHandler::Get()->LoadShader((string)"UntexturedShader", "Data/shaders/untextured.vert", "Data/shaders/untextured.frag");
	animatedProg = ShaderHandler::Get()->LoadShader((string)"ModelShader", "Data/shaders/lighting.vert", "Data/shaders/lighting.frag");

	grassTexture = LoadTexture("Data/textures/grass.jpg");
	sandTexture = LoadTexture("Data/textures/sand.jpg");

	//initialize bunny
	fbxBunny = new FBXFile();
	fbxBunny->load("Data/models/Bunny.fbx");
	CreateOpenGLBuffers(fbxBunny);
	
	//initialize pyro
	fbxPyro = new FBXFile();
	fbxPyro->load("Data/models/Pyro/pyro.fbx");
	fbxPyro->initialiseOpenGLTextures();
	CreateOpenGLBuffers(fbxPyro);

	//tweak bar init
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);

	octaves = 6;
	amplitude = 1.f;
	seed = 5;
	oldPersist = false;
	oldAmp = false;
	seedChanged = false;

	gui = TwNewBar("World Editor");
	TwAddVarRW(gui, "Terrain Seed", TW_TYPE_INT32, &seed, "");
	TwAddVarRW(gui, "Terrain Persistance", TW_TYPE_INT32, &octaves, "");
	TwAddVarRW(gui, "Terrain Amplitude", TW_TYPE_FLOAT, &amplitude, "");
	//property init
	oldPersist = octaves;
	oldAmp = amplitude;
	oldSeed = seed;

	return true;
}

float* ProceduralGen::GeneratePerlinData(int _dims, int _scale)
{
	return new float;
}

void ProceduralGen::Shutdown()
{
	//cleaning up fbx models
	CleanupOpenGLBuffers(fbxBunny);
	CleanupOpenGLBuffers(fbxPyro);

	//LAST
	TwDeleteAllBars();
	TwTerminate();
}

bool ProceduralGen::Update(double _dt)
{

	//persistance changes
	if (oldPersist != octaves)
	{
		PersistChanged = true;
	}
	oldPersist = octaves;
	if (PersistChanged == true)
	{
		GenerateTerrain(200);
		PersistChanged = false;
	}

	//amplitude changes
	if (oldAmp != amplitude)
	{
		ampChanged = true;
	}
	oldAmp = amplitude;
	if (ampChanged == true)
	{
		GenerateTerrain(200);
		ampChanged = false;
	}

	//seed changes
	if (oldSeed != seed)
	{
		seedChanged = true;
	}
	oldSeed = seed;
	if (seedChanged == true)
	{
		//GenerateGrid(200, 200);
		GenerateTerrain(200);
		seedChanged = false;
	}

	m_camera->Update(_dt);

	//performing animation updates
	FBXSkeleton* skeleton = fbxPyro->getSkeletonByIndex(0);
	FBXAnimation* animation = fbxPyro->getAnimationByIndex(0);

	float timer = animation->totalTime();

	skeleton->evaluate(animation, glfwGetTime());

	for (unsigned int boneIndex = 0; boneIndex < skeleton->m_boneCount; ++boneIndex)
	{
		skeleton->m_nodes[boneIndex]->updateGlobalTransform();
	}

	return true;
}

void ProceduralGen::Render()
{
	glUseProgram(shaderProg);

	int location = glGetUniformLocation(shaderProg, "ProjectionView");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_camera->GetProjectionView()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);
	location = glGetUniformLocation(shaderProg, "PerlinTexture");
	glUniform1i(location, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	location = glGetUniformLocation(shaderProg, "GrassTexture");
	glUniform1i(location, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sandTexture);
	location = glGetUniformLocation(shaderProg, "SandTexture");
	glUniform1i(location, 2);

	glBindVertexArray(buffers->VAO);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(untexturedProg);
	location = glGetUniformLocation(untexturedProg, "ProjectionView");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_camera->GetProjectionView()));

	//rendering the bunny
	for (unsigned int i = 0; i < fbxBunny->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxBunny->getMeshByIndex(i);
	
		unsigned int* glData = (unsigned int*)mesh->m_userData;
	
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	
	//setting up, animating and rendering the pyro
	glUseProgram(animatedProg);
	location = glGetUniformLocation(animatedProg, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	location = glGetUniformLocation(animatedProg, "LightDir");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 0.5f, 0)));
	location = glGetUniformLocation(animatedProg, "LightColour");
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(1, 1, 1)));
	location = glGetUniformLocation(animatedProg, "CameraPos");
	glUniform3fv(location, 1, glm::value_ptr(m_camera->GetPosition()));
	location = glGetUniformLocation(animatedProg, "SpecPow");
	glUniform1f(location, 28);

	//getting the skeleton and animation
	FBXSkeleton* skeleton = fbxPyro->getSkeletonByIndex(0);
	skeleton->updateBones();
	location = glGetUniformLocation(animatedProg, "bones");
	glUniformMatrix4fv(location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < fbxPyro->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbxPyro->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		//mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		glUniform1i(glGetUniformLocation(animatedProg, "diffuse"), 3);

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//LAST
	TwDraw();
}

int ProceduralGen::LoadTexture(std::string file)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load(file.c_str(), &imageWidth, &imageHeight, &imageFormat, STBI_default);

	unsigned int texture;

	switch (imageFormat)
	{
		case 3:
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case 4:
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		default:
			return 0;
			break;
	}

	return texture;

	stbi_image_free(data);
}

void ProceduralGen::GenerateTerrain(unsigned int rows)
{
	glm::vec2 dims(rows, rows);

	float *perlin_data = new float[(int)dims.x * (int)dims.y];
	float scale = (1.0f / dims.x) * 3;
	octaves = 6;
	for (int x = 0; x < rows; ++x)
	{
		for (int y = 0; y < rows; ++y)
		{
			amplitude = 1.f;
			float persistence = 0.3f;
			perlin_data[y * (int)dims.x + x] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(glm::vec3((float)x, (float)y, seed) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * (int)dims.x + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	glGenTextures(1, &perlinTexture);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, rows, rows, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// function to create a grid
void ProceduralGen::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];

	//Nested loop for looping through rows then columns for the grid
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);
			aoVertices[r * cols + c].normal = glm::vec4(0, 1, 0, 1);

			// create some arbitrary colour based off something
			// that might not be related to tiling a texture
			aoVertices[r * cols + c].uv = glm::vec2((r / (float)rows), (c / (float)cols));
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
	unsigned int index = 0;

	for (unsigned int r = 0; r < (rows - 1); ++r) {
		for (unsigned int c = 0; c < (cols - 1); ++c) {
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	unsigned int ibo;
	indexCount = (rows - 1) * (cols - 1) * 6;

	//Create VBO
	glGenBuffers(1, &buffers->VBO);

	//Create IBO
	glGenBuffers(1, &ibo);

	//Create VAO
	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);

	//Bind VBO and define size of data
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//pos
	glEnableVertexAttribArray(1);//normals
	glEnableVertexAttribArray(2);//texcoords

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)+sizeof(glm::vec4)));

	//Bind IBO and define size of data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	//Unbind the VAO (which unbinds its VBO and IBO)
	glBindVertexArray(0);

	//Delete our temp indecies
	delete[] auiIndices;

	//Delete our temp verticies
	delete[] aoVertices;
}

void ProceduralGen::CreateOpenGLBuffers(FBXFile* _model)
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

void ProceduralGen::CleanupOpenGLBuffers(FBXFile* _model)
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