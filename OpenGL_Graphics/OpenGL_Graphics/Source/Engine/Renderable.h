#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;

class Camera;
class Material;

class Renderable
{
public:

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
		glm::vec2 uv;
	};

	Renderable();
	~Renderable();

	virtual void Create() = 0;

	virtual void Render(Camera* _camera, glm::mat4 _transform); //currently pasing in render location. Full node system to implement later.

	bool PrepareToRender(glm::mat4 _transform);

	inline const unsigned int& GetVAO() const { return VAO; }
	inline const unsigned int& GetVBO() const { return VBO; }
	inline const unsigned int& GetIBO() const { return IBO; }

	void SetMaterial(Material* _material);
	void SetMaterial(string& _material);
	inline Material* GetMaterial() { return material; }

protected: 

	unsigned int numberOfIndices;
	unsigned int numberOfVerts;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

private: //private members hidden from child classes

	Material* material;
	string materialName;

};