#include "Scene.h"
#include "Engine\MobileCamera.h"
#include "Engine\ShaderHandler.h"

bool Scene::Startup()
{
	MobileCamera* camera = new MobileCamera(100.0f, 0.1f);
	camera->SetInputWindow(window);
	camera->SetupPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 10000.0f);
	camera->LookAt(glm::vec3(100, 100, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_camera = camera;

	//SHADERS
	//procedural gen
	terrainShader = ShaderHandler::Get()->LoadShader((string)"TerrainShader", "Data/shaders/proceduralGen.vert", "Data/shaders/proceduralGen.frag");
	planeShader = ShaderHandler::Get()->LoadShader((string) "PlaneShader", "Data/shaders/bluePlane.vert", "Data/shaders/bluePlane.frag");
	//particles

	//objects


	//AntBar init
	/*TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);*/

	tweakBar = TwNewBar("World_Editor");
	TwAddVarRW(tweakBar, "Terrain Amplitude", TW_TYPE_FLOAT, &editAmplitude, "");
	TwAddVarRW(tweakBar, "Terrain Persistance", TW_TYPE_FLOAT, &editScale, "");

	//variable init
	barChanged = false;
	editAmplitude = 3.0f;
	editScale = 8;
	perlinSeed = 10;

	GenerateTerrain(64, editScale, editAmplitude);
	CreatePlane(64);

	return true;
}

void Scene::Shutdown()
{
	

	//LAST
	TwDeleteAllBars();
	TwTerminate();
}

bool Scene::Update(double _dt)
{
	m_camera->Update(_dt);

	if (oldAmp != editAmplitude || oldPers != editScale)
	{
		barChanged = true;
	}

	oldAmp = editAmplitude;
	oldPers = editScale;

	if (barChanged == true)
	{
		GenerateTerrain(64, editScale, editAmplitude);
		barChanged = false;
	}

	return true;
}

void Scene::Render()
{
	//TERRAIN
	glUseProgram(terrainShader);
	int location = glGetUniformLocation(terrainShader, "ProjectionView");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_camera->GetProjectionView()));
	//texturing
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);
	location = glGetUniformLocation(terrainShader, "perlinTexture");
	glUniform1i(location, 0);
	glBindVertexArray(buffers->VAO);
	//drawing triangles
	unsigned int indexCount = (gridRows - 1) * (gridColumns - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	//PLANE
	location = glGetUniformLocation(planeShader, "ProjectionView");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_camera->GetProjectionView()));
	//texturing
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	location = glGetUniformLocation(planeShader, "planeTex");
	glUniform1i(location, 0);
	glBindVertexArray(buffers->VAO);
	//drawing triangles
	indexCount = (gridRows - 1) * (gridColumns - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//PARTICLES

	//OBJECTS

	//LAST
	TwDraw();
}

void Scene::GenerateTerrain(int _dimensions, int _terrScale, float _terrAmplitude)
{
	int resizeNum = _dimensions * _dimensions;

	vertices.resize(resizeNum);

	for (unsigned int r = 0; r < _dimensions; ++r)
	{
		for (unsigned int c = 0; c < _dimensions; ++c)
		{
			vertices[r * _dimensions + c].position = glm::vec4((float)c, 0, (float)r, 1);

			glm::vec3 colour = glm::vec3(1, 1, 1);

			vertices[r * _dimensions + c].normal = glm::vec3(0, 1, 0);
			vertices[r * _dimensions + c].uv = glm::vec2((float)c / _dimensions, (float)r / _dimensions);
		}
	}

	//defining index count based of quad count (2 tri's = 1 quad)
	indices = new unsigned int[(_dimensions - 1) * (_dimensions - 1) * 6];
	unsigned int index = 0;

	for (unsigned int r = 0; r < (_dimensions - 1); ++r)
	{
		for (unsigned int c = 0; c < (_dimensions - 1); ++c)
		{
			//tri 1
			indices[index++] = r * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + (c + 1);

			//tri 2
			indices[index++] = r * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + (c + 1);
			indices[index++] = r * _dimensions + (c + 1);
		}
	}

	//save function parametres for member varibles for external use
	gridRows = _dimensions;
	gridColumns = _dimensions;

	//generating perlin data
	float* perlinData = GeneratePerlinData(_dimensions, _terrScale, _terrAmplitude);

	GenerateNormals();

	CreateOpenGLBuffers(vertices);

	//generating noise texture
	glGenTextures(1, &perlinTexture);
	glBindTexture(GL_TEXTURE_2D, perlinTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlinData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

float* Scene::GeneratePerlinData(int _dims, int _scale, float _amplitude)
{
	float* perlinData = new float[_dims * _dims];

	float scale = (1.0f / _dims) * _scale;
	float octaves = 6;

	for (int x = 0; x < _dims; ++x)
	{
		for (int y = 0; y < _dims; ++y)
		{
			float amplitude = _amplitude;
			float persistance = 0.3f;
			perlinData[y * _dims + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlinSample = glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;

				perlinData[y * _dims + x] += perlinSample * amplitude;
				amplitude *= persistance;
			}
		}
	}

	return perlinData;
}

void Scene::CreatePlane(int _dimensions)
{
	Vertex* vertices = new Vertex[_dimensions * _dimensions];
	for (unsigned int r = 0; r < _dimensions; ++r)
	{
		for (unsigned int c = 0; c < _dimensions; ++c)
		{
			vertices[r * _dimensions + c].position = glm::vec4((float)c, 0, (float)r, 1);

			glm::vec3 colour = glm::vec3(0, 0, 1);

			vertices[r * _dimensions + c].normal = glm::vec3(0, 1, 0);
			vertices[r * _dimensions + c].uv = glm::vec2((float)c / _dimensions, (float)r / _dimensions);
		}
	}

	//defining index count based of quad count (2 tri's = 1 quad)
	unsigned int* indices = new unsigned int[(_dimensions - 1) * (_dimensions - 1) * 6];
	unsigned int index = 0;

	for (unsigned int r = 0; r < (_dimensions - 1); ++r)
	{
		for (unsigned int c = 0; c < (_dimensions - 1); ++c)
		{
			//tri 1
			indices[index++] = r * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + (c + 1);

			//tri 2
			indices[index++] = r * _dimensions + c;
			indices[index++] = (r + 1) * _dimensions + (c + 1);
			indices[index++] = r * _dimensions + (c + 1);
		}
	}

	float* planeData = new float(1);

	//generating noise texture
	glGenTextures(1, &planeTexture);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, planeData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//generating VAO, VBO and IBO
	glGenBuffers(1, &buffers->VBO);
	glGenBuffers(1, &buffers->IBO);

	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);

	//binding and filling VBO
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);
	glBufferData(GL_ARRAY_BUFFER, (_dimensions * _dimensions) * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	//binding and filling IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (_dimensions - 1) * (_dimensions - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//binding VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//save function parametres for member varibles for external use
	gridRows = _dimensions;
	gridColumns = _dimensions;

	delete[] vertices;
	delete[] indices;
}

void Scene::CreateOpenGLBuffers(std::vector<Vertex> _verts)
{
	glGenVertexArrays(1, &buffers->VAO);
	glBindVertexArray(buffers->VAO);

	glGenBuffers(1, &buffers->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* _verts.size(), _verts.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1); //normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2); //uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glGenBuffers(1, &buffers->IBO); //indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* ((gridRows - 1) * (gridColumns - 1) * 6), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Scene::GenerateNormals()
{
	Vertex* vert1;
	Vertex* vert2;
	Vertex* vert3;

	//calculate terrain normals
	unsigned int girdIndices = (64 - 1) * (64 - 1) * 6;
	for (unsigned int i = 0; i < girdIndices; i += 3)
	{
		vert1 = &vertices[indices[i + 2]];
		vert2 = &vertices[indices[i + 1]];
		vert3 = &vertices[indices[i]];

		//calculate face normal
		glm::vec3 d1(vert3->position - vert1->position);
		glm::vec3 d2(vert2->position - vert1->position);

		glm::vec3 crossProduct = glm::cross(d1, d2);

		glm::vec3 normalized = glm::normalize(crossProduct);

		vert1->normal = normalized;
		vert2->normal = normalized;
		vert3->normal - normalized;
	}
}

void Scene::DrawNormals()
{
	glm::vec4 lineColour(1.0f, 0.0f, 0.0f, 1.0f);

	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		glm::vec3 vertPos = vertices[i].position.xyz;
		glm::vec3 vertNorms = vertices[i].normal;

		//Gizmos::addLine(vertPos, vertPos + vertNorms, lineColour);
	}
}