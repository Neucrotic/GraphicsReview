#include "TextureLoader.h"
#include "gl_core_4_4.h"
#include "stb_image.h"

#include <list>

TextureLoader::~TextureLoader()
{
	//no for loop used due to incomparitablity
	//loop through the texture map and delete any textures found
	auto iter = textureMap.begin();
	while (iter != textureMap.end())
	{
		glDeleteTextures(1, &(iter->second));
		iter++;
	}
}

unsigned int TextureLoader::LoadTexture(string& _textureName)
{
	//if the texture already exists, find and return it
	if (DoesTextureExist(_textureName))
	{
		return textureMap[_textureName];
	}

	//else generate a texture and add it to the texture map
	int imageWidth = 0;
	int imageHeight = 0;
	int imageFormat = 0;

	unsigned char* data = stbi_load(_textureName.c_str(), &imageWidth, &imageHeight, &imageFormat, STBI_default);

	unsigned int textureID = 0;

	//generating and binding textures
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //free up char* 

	textureMap[_textureName] = textureID; //set the textureID relative to the _texureName

	return textureID; //return the textureID value
}

unsigned int TextureLoader::GetTexture(string& _textureName) const
{
	if (DoesTextureExist(_textureName))
	{
		//returns a refereance to _textureName NOTE: only called once we know it exists with DoesTextureExist
		return textureMap.at(_textureName);
	}
	//only reaches this point if texture can't be found
	return NULL;
}

void TextureLoader::UnloadTexture(string& _textureName)
{
	if (!DoesTextureExist(_textureName))
		return; //texture not loaded

	unsigned int textureID = textureMap[_textureName]; //assigning textureID to the texutre we are unloading

	glDeleteTextures(1, &textureID);

	textureMap.erase(_textureName);
}

unsigned int TextureLoader::LoadCubeMap(string& _folderName, string _extension, CUBE_MAP_NAMING _naming)
{
	if (cubeMap[_folderName]) //if the file already exists return it, no need to do any loading
	{
		return cubeMap[_folderName]; 
	}

	unsigned int textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	switch (_naming) //check which type of loading to do with the cube map
	{
	case TextureLoader::SEQUENCE:
		LoadCubeMapSequencial(_folderName, _extension);
		break;
	case TextureLoader::POSITION:
		LoadCubeMapPosition(_folderName, _extension);
		break;
	default:
		break;
	}

	//wrapping and binding the texuture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	cubeMap[_folderName] = textureID;

	return textureID;
}

void TextureLoader::LoadCubeMapSequencial(string& _folderName, string _extension)
{
	//iterate through for each face on the cube and load the texture to the cube map
	for (int i = 0; i < 6; ++i)
	{
		int imageWidth;
		int imageHeight;
		int imageFormat;

		string filename = _folderName;
		filename += "/";

		char buffer[2];
		itoa(i, buffer, 10);

		//adding the extension to the file buffer
		filename += buffer;
		filename += ".";
		filename += _extension;

		unsigned char* data = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageFormat, STBI_default);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
}

void TextureLoader::LoadCubeMapPosition(string& _folderName, string _extention)
{
	list<string> files;
	//pushing in strings, use 2 letters for consistency 
	files.push_back("RT"); //right
	files.push_back("LF"); //left
	files.push_back("UP"); //up
	files.push_back("DN"); //down
	files.push_back("BK"); //back
	files.push_back("FR"); //front

	unsigned int counter = 0;
	for each(string name in files)
	{
		int imageWidth = 0;
		int imageHeight = 0;
		int imageFormat = 0;

		//adding the filename, map position and extension to the file
		string filename = _folderName;
		filename += "/";
		filename += name;
		filename += ".";
		filename += _extention;

		unsigned char* data = stbi_load(filename.c_str(), &imageWidth, &imageHeight, &imageFormat, STBI_default); //stroing image data in char pointer

		//loading up image with appropriate values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + counter, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		counter++;
	}
}

bool TextureLoader::DoesTextureExist(string& _textureName) const
{
	//if the look up key (param) cannot be found in the map map.end will be returned
	if (textureMap.find(_textureName) == textureMap.end())
		return false;

	return true;
}
