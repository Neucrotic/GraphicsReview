#pragma once
#include <string>
#include <map>
#include <glm/glm.hpp>

using namespace std;

class ShaderHandler
{
public:

	~ShaderHandler();

	static ShaderHandler* Init();
	static void Deinit();
	static ShaderHandler* Get();

	unsigned int LoadShader(string& _shaderName,
		const char* _vertexShader,
		const char* _pixelShader = nullptr,
		const char* _geomertyShader = nullptr,
		const char* _tessellationControlShader = nullptr,
		const char* _tessellationEvaluationShader = nullptr);

	unsigned int GetShader(string& _shaderName);
	inline unsigned int GetErrorShader() { return errorShader; }
	unsigned int GetScreenAlignedShader();

	//potentally rewrite these to not do lookups each frame i.e. if found dont look etc.
	void SetProjectionMatrix(glm::mat4 _projectionMatrix);
	void SetViewMatrix(glm::mat4 _viewMatrix);
	void SetEyePosition(glm::vec4 _eyePosition);

private:

	ShaderHandler();

	bool DoesShaderExist(string& _materialName);
	unsigned int LoadShader(const char* _name, unsigned int _type);

	//Default shader setup - must exist for engine to work
	void CreateErrorShader();
	unsigned char* FileToBuffer(const char* _name); //uses usigned char to give ASCII values 0-255
	unsigned int CreateProgram(unsigned int _vertShader, 
							   unsigned int _fragShader,
							   unsigned int _geomShader = 0, 
							   unsigned int _tesselEvalShader = 0, 
							   unsigned int _tesselControlShader = 0);

	map<string, unsigned int> shaderMap;

	unsigned int errorShader;

	//potentially remove and reqire certain functions
	static ShaderHandler* instance;

};