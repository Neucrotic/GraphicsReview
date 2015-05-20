#pragma once
#include <map>
#include <string>

using namespace std;

class TextureLoader
{
public: 

	enum CUBE_MAP_NAMING //used for mapping texture to cube
	{
		SEQUENCE,
		POSITION
	};

	TextureLoader() {}; //0 initialisation needed = no body
	~TextureLoader();

	unsigned int LoadTexture(std::string& _texureName);
	unsigned int GetTexture(string& _textureName) const;
	
	void UnloadTexture(string& _textureName);

	unsigned int LoadCubeMap(string& _folderName, string _extension, CUBE_MAP_NAMING _naming);

private:

	void LoadCubeMapSequencial(string& _folderName, string _extention);
	void LoadCubeMapPosition(string& _folderName, string _extention);

	bool DoesTextureExist(string& _textureName) const;
	
	map<string, unsigned int> textureMap;
	map<string, unsigned int> cubeMap;

};