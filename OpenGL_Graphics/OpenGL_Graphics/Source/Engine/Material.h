#pragma once
#include <map>
#include "gl_core_4_4.h"
#include <glm/glm.hpp>

class Material
{
public:
	enum TextureTypes
	{
		DIFFUSE,
		AMBIENT,
		GLOW,
		SPECULAR,
		GLOSS,
		NORMAL,
		ALPHA,
		DISPLACEMENT,
		CUBE
	};

	char name[MAX_PATH];

	glm::vec4 ambient; //RBG + ambient stored in A
	glm::vec4 diffuse; //RGBA
	glm::vec4 specular; //RGB + Shininess/gloss stored in A
	glm::vec4 emissive; //RBG + emissive factor stored in A

	inline void SetTexture(TextureTypes _texType, unsigned int _textureID) { textureMap[_texType] = _textureID; }

	void SetShader(unsigned int _shaderID); //NOTE: will also bind the shader
	inline const unsigned int GetShaderID() { return shaderID; }

	void SetAsActiveMaterial(); //look into establishing a render pipeline

	//make sure these used cached lookup values for best performance
	void SetShaderWorldMatrix(glm::mat4& _worldMatrix);
	void SetShaderProjectionViewMatrix(glm::mat4& _projectionMatrix);

private:

	std::map<TextureTypes, unsigned int> textureMap;

	unsigned int shaderID;

	int diffuseShaderLocation;
	int ambientShaderLocation;
	int specularShaderLocation;
	int emissiveShaderLocation;

	int worldMatrixShaderLocation;
	int projectionViewMatrixShaderLocation;

};