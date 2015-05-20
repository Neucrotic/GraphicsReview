#include "MaterialHandler.h"
#include "TextureLoader.h"
#include "ShaderHandler.h"
#include "Material.h"
#include <assert.h>

MaterialHandler* MaterialHandler::instance = nullptr; //initialise as null ptr for constructor purposes

MaterialHandler::MaterialHandler() : errorMaterial(nullptr) //public
{
	textureLoader = new TextureLoader();
}

MaterialHandler::~MaterialHandler() //private
{
	//loop through and delete loaded materials
	for (auto iter = materialMap.begin(); iter != materialMap.end(); ++iter)
	{
		delete iter->second;
	}
	
	delete textureLoader;

	delete errorMaterial;
}

MaterialHandler* MaterialHandler::Init()
{
	if (instance == nullptr)
	{
		instance = new MaterialHandler();
		return instance;
	}

	return instance;
}

void MaterialHandler::Deinit()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = NULL;
	}
}

MaterialHandler* MaterialHandler::Get()
{
	return instance;
}

Material* MaterialHandler::CreateNewMaterial(string& _name)
{
	Material* material = new Material; //no constructor exists

	materialMap[_name] = material;
	material->SetShader(defaultShader);

	return material;
}

Material* MaterialHandler::CreateNewMaterial(tinyobj::material_t* _material, string& _texturePath)
{
	assert(_material != nullptr);
	Material* material = new Material; //no constructor exists
	materialMap[_material->name] = material;

	strcpy(material->name, _material->name.c_str());

	material->ambient = glm::vec4(_material->ambient[0], _material->ambient[1], _material->ambient[2], 1);
	material->diffuse = glm::vec4(_material->diffuse[0], _material->diffuse[1], _material->diffuse[2], 1);
	material->specular = glm::vec4(_material->specular[0], _material->specular[1], _material->specular[2], 1);
	material->emissive = glm::vec4(_material->emission[0], _material->emission[1], _material->emission[2], 1);

	if (_material->diffuse_texname != "")
	{
		material->SetTexture(Material::DIFFUSE, textureLoader->LoadTexture(_texturePath + string(_material->diffuse_texname)));
	}

	if (_material->ambient_texname != "")
	{
		material->SetTexture(Material::AMBIENT, textureLoader->LoadTexture(_texturePath + string(_material->ambient_texname)));
	}

	if (_material->normal_texname != "")
	{
		material->SetTexture(Material::NORMAL, textureLoader->LoadTexture(_texturePath + string(_material->normal_texname)));
	}

	if (_material->specular_texname != "")
	{
		material->SetTexture(Material::SPECULAR, textureLoader->LoadTexture(_texturePath + string(_material->specular_texname)));
	}

	material->SetShader(defaultShader);

	return material;
}

Material* MaterialHandler::GetMaterial(string& _materialName)
{
	if (DoesMaterialExist(_materialName))
	{
		return materialMap.at(_materialName);
	}

	return NULL;
}

void MaterialHandler::ConfigureOpenGLTextureSlots(unsigned int _shaderID)
{
	glUseProgram(_shaderID);
	//tell the shader where to find all textures if it uses them
	//diffuse texture
	GLint shaderHandler = glGetUniformLocation(_shaderID, "diffuseTexture");
	glUniform1i(shaderHandler, Material::DIFFUSE);

	//ambient texture
	shaderHandler = glGetUniformLocation(_shaderID, "ambientTexture");
	glUniform1i(shaderHandler, Material::AMBIENT);

	//glow texture
	shaderHandler = glGetUniformLocation(_shaderID, "glowTexture");
	glUniform1i(shaderHandler, Material::GLOW);

	//specular texture
	shaderHandler = glGetUniformLocation(_shaderID, "specularTexture");
	glUniform1i(shaderHandler, Material::SPECULAR);

	//gloss texture
	shaderHandler = glGetUniformLocation(_shaderID, "glossTexture");
	glUniform1i(shaderHandler, Material::GLOSS);

	//normalTexture
	shaderHandler = glGetUniformLocation(_shaderID, "normalTexture");
	glUniform1i(shaderHandler, Material::NORMAL);

	//alphaTexture
	shaderHandler = glGetUniformLocation(_shaderID, "alphaTexture");
	glUniform1i(shaderHandler, Material::ALPHA);

	//displacementTexture
	shaderHandler = glGetUniformLocation(_shaderID, "displacementTexture");
	glUniform1i(shaderHandler, Material::DISPLACEMENT);

	//cubeMap
	shaderHandler = glGetUniformLocation(_shaderID, "cubeMap");
	glUniform1i(shaderHandler, Material::CUBE);
}

Material* MaterialHandler::GetDefaultErrorMaterial()
{
	if (errorMaterial != nullptr)
	{
		return errorMaterial;
	}
	else
	{
		errorMaterial = new Material();
		if (ShaderHandler::Get() != nullptr)
		{
			errorMaterial->SetShader(ShaderHandler::Get()->GetErrorShader());
		}
		return errorMaterial;
	}

	return nullptr;
}

bool MaterialHandler::DoesMaterialExist(string& _materialName)
{
	if (materialMap.find(_materialName) == materialMap.end())
	{
		return false;
	}

	return true;
}