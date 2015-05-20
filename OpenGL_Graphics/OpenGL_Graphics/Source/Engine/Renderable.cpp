#include "Renderable.h"
#include "gl_core_4_4.h"

#include "Camera.h"
#include "Material.h"
#include "MaterialHandler.h"

Renderable::Renderable()
	: VAO(0),
	VBO(0),
	IBO(0),
	numberOfIndices(0),
	numberOfVerts(0),
	material(nullptr),
	materialName("")
{

}

Renderable::~Renderable()
{
	if (VAO == 0)
	{
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO == 0)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (IBO == 0)
	{
		glDeleteBuffers(1, &IBO);
	}
}

void Renderable::Render(Camera* _camera, glm::mat4 _transform)
{
	if (!PrepareToRender(_transform))
		return;

	glBindVertexArray(VAO);

	//render the object
	glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

bool Renderable::PrepareToRender(glm::mat4 _transform)
{
	//0 indices indicates nothing to render
	if (numberOfIndices == 0)
		return false;

	if (material == nullptr) //will happen on first render
	{
		if (MaterialHandler::Get()) //attempt to get material
		{
			material = MaterialHandler::Get()->GetMaterial(materialName);
		}
		if (MaterialHandler::Get() && material == nullptr) //if still null, set to error shader
		{
			material = MaterialHandler::Get()->GetDefaultErrorMaterial();
		}
	}

	if (material != nullptr)
	{
		material->SetAsActiveMaterial();
	}

	//set transform
	material->SetShaderWorldMatrix(_transform);

	return true;
}

void Renderable::SetMaterial(Material* _material)
{
	material = _material;
	materialName = material->name;
}

void Renderable::SetMaterial(string& _material)
{
	materialName = _material;
	material = MaterialHandler::Get()->GetMaterial(_material);
}