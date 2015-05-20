#pragma once
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <string>
#include "tiny_obj_loader.h"
#include "Engine/Renderable.h"

using namespace std;

class ObjectMesh : public Renderable
{
public:

	struct Vertex
	{
		enum Offsets //initialise enum here
		{
			PositionOffset = 0,
			ColourOffset = PositionOffset + sizeof(glm::vec4),
			NormalOffset = ColourOffset + sizeof(glm::vec4),
			TexCoordOffset = NormalOffset + sizeof(glm::vec3),
			BinormalOffset = TexCoordOffset + sizeof(glm::vec2),
			TangentOffset = BinormalOffset + sizeof(glm::vec3)
		};

		glm::vec4 position;
		glm::vec4 colour;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 binormal;
		glm::vec3 tangent;
	};

	ObjectMesh(string& _filePath);

	void Create();

private:

	void CreateVAOFromShapes(vector<tinyobj::shape_t>& _shapes);
	void CreateVertexArrayFromShapes(vector<tinyobj::shape_t>& _shapes, Vertex* _vertices);

	void CreateMaterialFromMaterials(vector<tinyobj::material_t>& _materials);

	string GetMaterialDirectory();

	void CalculateBinormalsAndTangents(vector<float>& _position, 
				vector<float>& _normals, vector<float>& _texCoords, 
				vector<float>& _binormals, vector<float>& _tangents);
	void CalculateBinormalsAndTangents(Vertex* _vertices, vector<unsigned int>& _indices);


	string filePath;
	
};