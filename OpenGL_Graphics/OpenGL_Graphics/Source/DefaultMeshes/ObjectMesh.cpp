#include "ObjectMesh.h"
#include "Engine/ShaderHandler.h"
#include "Engine/Material.h"
#include "Engine/MaterialHandler.h"

ObjectMesh::ObjectMesh(string& _filePath)
{
	filePath = _filePath;

	Create();
}

void ObjectMesh::Create()
{
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string directoryPath = GetMaterialDirectory();
	
	string error = tinyobj::LoadObj(shapes, materials, filePath.c_str(), directoryPath.c_str());

	if (error == "" && !shapes.empty())
	{
		CreateVAOFromShapes(shapes);
	}

	if (error == "" && !materials.empty())
	{
		CreateMaterialFromMaterials(materials);
	}
}

void ObjectMesh::CreateVAOFromShapes(vector<tinyobj::shape_t>& _shapes)
{
	//initially load first shape
	numberOfVerts = _shapes[0].mesh.positions.size() / 3; //why divide by 3?
	numberOfIndices = _shapes[0].mesh.indices.size();

	Vertex* vertices = new Vertex[numberOfVerts];

	CreateVertexArrayFromShapes(_shapes, vertices);

	vector<unsigned int>& indices = _shapes[0].mesh.indices;

	//generate, bind and enable buffers
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndices * sizeof(unsigned int), &(indices[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::ColourOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::NormalOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::TexCoordOffset);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::BinormalOffset);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0) + Vertex::TangentOffset);

	//unbind vertex array
	glBindVertexArray(0);

	delete[] vertices;
}

void ObjectMesh::CreateVertexArrayFromShapes(vector<tinyobj::shape_t>& _shapes, Vertex* _vertices)
{
	unsigned int index = 0;
	unsigned int UVIndex = 0;

	vector<float>& positions = _shapes[0].mesh.positions;
	vector<float>& normals = _shapes[0].mesh.normals;
	vector<float>& texCoords = _shapes[0].mesh.texcoords;

	for (unsigned int i = 0; i < numberOfVerts; ++i)
	{
		_vertices[i].position = glm::vec4(positions[index], positions[index + 1], positions[index + 2], 1.0f);
		_vertices[i].colour = glm::vec4(1, 1, 1, 1); //defaulting to white
		_vertices[i].normal = glm::vec3(normals[index], normals[index + 1], normals[index + 2]);
		_vertices[i].uv = glm::vec2(texCoords[UVIndex], 1 - texCoords[UVIndex + 1]);

		index += 3;
		UVIndex += 2;
	}

	CalculateBinormalsAndTangents(_vertices, _shapes[0].mesh.indices);
}

void ObjectMesh::CreateMaterialFromMaterials(vector<tinyobj::material_t>& _materials)
{
	string directoryPath = GetMaterialDirectory();

	//load only the first material (for now)
	tinyobj::material_t& mat = _materials[0];

	SetMaterial(MaterialHandler::Get()->CreateNewMaterial(&mat, directoryPath));
	GetMaterial()->SetShader(ShaderHandler::Get()->LoadShader((string)"OBJShader", "Data/shaders/lighting.vert", "Data/shaders/lighting.frag"));
}

string ObjectMesh::GetMaterialDirectory()
{
	string directoryPath;

	size_t last_slash_idx = filePath.rfind('\\');

	if (last_slash_idx == string::npos)
	{
		last_slash_idx = filePath.rfind('/');
	}

	if (string::npos != last_slash_idx)
	{
		directoryPath = filePath.substr(0, last_slash_idx);
	}
	else //no drectory, use current
	{
		directoryPath = "";
	}

	return directoryPath + "/";
}

void ObjectMesh::CalculateBinormalsAndTangents(vector<float>& _position,
						vector<float>& _normals, vector<float>& _texCoords,
						vector<float>& _binormals, vector<float>& _tangents)
{
	for (unsigned int i = 0, j = 0; i < _position.size(); i += 9, j += 6) //add 6 and 9 due to logic within loop
	{
		//vertices shortcuts
		glm::vec3 & v0 = glm::vec3(_position[i + 0], _position[i + 1], _position[i + 2]);
		glm::vec3 & v1 = glm::vec3(_position[i + 3], _position[i + 4], _position[i + 5]);
		glm::vec3 & v2 = glm::vec3(_position[i + 6], _position[i + 7], _position[i + 8]);

		//UV shortcuts
		glm::vec2 & uv0 = glm::vec2(_texCoords[j + 0], _texCoords[j + 1]);
		glm::vec2 & uv1 = glm::vec2(_texCoords[j + 2], _texCoords[j + 3]);
		glm::vec2 & uv2 = glm::vec2(_texCoords[j + 4], _texCoords[j + 5]);

		//edges of the triangle = position delta (triangle maths)
		//vertice delta
		glm::vec3 deltaPos1 = v1 - v0; 
		glm::vec3 deltaPos2 = v2 - v0;

		//UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tan = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 biNorm = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		for (int k = 0; k < 3; ++k)
		{
			_tangents.push_back(tan.x);
			_tangents.push_back(tan.y);
			_tangents.push_back(tan.z);
		}

		for (int k = 0; k < 3; ++k)
		{
			_binormals.push_back(biNorm.x);
			_binormals.push_back(biNorm.y);
			_binormals.push_back(biNorm.z);
		}
	}
}

void ObjectMesh::CalculateBinormalsAndTangents(Vertex* _vertices, vector<unsigned int>& _indices)
{
	for (int index = 0; index < _indices.size(); index += 3)
	{
		int i0 = _indices[index];
		int i1 = _indices[index + 1];
		int i2 = _indices[index + 2];

		glm::vec3 v0 = _vertices[i0].position.xyz;
		glm::vec3 v1 = _vertices[i1].position.xyz;
		glm::vec3 v2 = _vertices[i2].position.xyz;

		glm::vec2 uv0 = _vertices[i0].uv;
		glm::vec2 uv1 = _vertices[i1].uv;
		glm::vec2 uv2 = _vertices[i2].uv;

		//edgesof triangle = position delta
		//vertice delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		//UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tan = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
		glm::vec3 biNorm = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		_vertices[i0].binormal = biNorm;
		_vertices[i1].binormal = biNorm;
		_vertices[i2].binormal = biNorm;

		_vertices[i0].tangent = tan;
		_vertices[i1].tangent = tan;
		_vertices[i2].tangent = tan;
	}
}