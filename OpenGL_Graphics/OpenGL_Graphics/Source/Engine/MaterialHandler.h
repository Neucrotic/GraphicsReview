#pragma once
#include "tiny_obj_loader.h"
#include <string>
#include <map>

using namespace std;

class Material;
class TextureLoader;

class MaterialHandler
{
public:

	~MaterialHandler();

	static MaterialHandler* Init();
	static void Deinit();
	static MaterialHandler* Get();

	Material* CreateNewMaterial(string& _name);
	Material* CreateNewMaterial(tinyobj::material_t* _material, string& _texturePath);

	Material* GetMaterial(string& _materialName);

	static void ConfigureOpenGLTextureSlots(unsigned int _shaderID);

	Material* GetDefaultErrorMaterial();
	Material* GetDefaultScreenAlignedMaterial();

	inline void SetDefaultShader(unsigned int _shaderID) { defaultShader = _shaderID; }

	inline TextureLoader* GetTextureLoader() { return textureLoader; }

private:

	MaterialHandler();

	bool DoesMaterialExist(string& _materialName);

	static MaterialHandler* instance;

	//used to load all the textures used by the materials
	TextureLoader* textureLoader;

	map<string, Material*> materialMap;

	unsigned int defaultShader;
	Material* errorMaterial;

};