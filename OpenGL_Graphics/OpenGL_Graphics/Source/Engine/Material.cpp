#include "Material.h"
#include "MaterialHandler.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

void Material::SetShader(unsigned int _shaderID)
{
	shaderID = _shaderID; //set shaderID

	//fetch uniform data from shaders for shader locations
	diffuseShaderLocation = glGetUniformLocation(shaderID, "diffuseTexture");
	ambientShaderLocation = glGetUniformLocation(shaderID, "ambientTexture");
	specularShaderLocation = glGetUniformLocation(shaderID, "specularTexture");
	emissiveShaderLocation = glGetUniformLocation(shaderID, "emissiveTexture");

	//fetch uniform data from shaders for matrix locations
	worldMatrixShaderLocation = glGetUniformLocation(shaderID, "World");
	projectionViewMatrixShaderLocation = glGetUniformLocation(shaderID, "ProjectionView");

	MaterialHandler::Get()->ConfigureOpenGLTextureSlots(shaderID);
}

void Material::SetAsActiveMaterial()
{
	glUseProgram(shaderID);

	glUniform4fv(diffuseShaderLocation, 1, glm::value_ptr(diffuse));
	glUniform4fv(ambientShaderLocation, 1, glm::value_ptr(ambient));
	glUniform4fv(specularShaderLocation, 1, glm::value_ptr(specular));
	glUniform4fv(emissiveShaderLocation, 1, glm::value_ptr(emissive));

	//binding to the correct textures
	for (auto iter = textureMap.begin(); iter != textureMap.end(); ++iter)
	{
		if (iter->second >= 0)
		{
			glActiveTexture(GL_TEXTURE0 + iter->first);
			if (iter->first != CUBE)
			{
				glBindTexture(GL_TEXTURE_2D, iter->second);
			}
			else
			{
				glBindTexture(GL_TEXTURE_CUBE_MAP, iter->second);
			}
		}
	}
}

void Material::SetShaderWorldMatrix(glm::mat4& _worldMatrix)
{
	glUseProgram(shaderID); //bind shader program

	glUniformMatrix4fv(worldMatrixShaderLocation, 1, false, glm::value_ptr(_worldMatrix)); //set the uniform to passed in value
}

void Material::SetShaderProjectionViewMatrix(glm::mat4& _projectionViewMatrix)
{
	glUseProgram(shaderID); //bind shader program

	glUniformMatrix4fv(projectionViewMatrixShaderLocation, 1, false, glm::value_ptr(_projectionViewMatrix)); //set the uniform to passed in value
}