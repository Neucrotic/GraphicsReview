#include "gl_core_4_4.h"
#include <glm/ext.hpp>
#include "ShaderHandler.h"

ShaderHandler* ShaderHandler::instance = nullptr; //initialise instance as nullptr unless init functions called

ShaderHandler::ShaderHandler() : errorShader(0)//private
{
	CreateErrorShader();
}

ShaderHandler::~ShaderHandler() //public 
{
	for (auto iter = shaderMap.begin(); iter != shaderMap.end(); ++iter)
	{
		glDeleteShader(iter->second);
	}

	glDeleteShader(errorShader);
}

ShaderHandler* ShaderHandler::Init() //check for nullptr
{
	if (instance == nullptr)
	{
		instance = new ShaderHandler();
		return instance;
	}

	return instance;
}

void ShaderHandler::Deinit()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = NULL;
	}
}

ShaderHandler* ShaderHandler::Get() //check for nullptr
{
	return instance;
}

unsigned int ShaderHandler::LoadShader(std::string& _shaderName,
	const char* _vertexShader,
	const char* _pixelShader,
	const char* _geomertyShader,
	const char* _tessellationControlShader,
	const char* _tessellationEvaluationShader)
{
	GLuint vs = LoadShader(_vertexShader, GL_VERTEX_SHADER);
	GLuint fs = 0;
	GLuint gs = 0;
	GLuint tcs = 0;
	GLuint tes = 0;


	if (_pixelShader != nullptr)
	{
		fs = LoadShader(_pixelShader, GL_FRAGMENT_SHADER);
	}

	if (_geomertyShader != nullptr)
	{
		gs = LoadShader(_geomertyShader, GL_GEOMETRY_SHADER);
	}
	
	if (_tessellationControlShader != nullptr)
	{
		tcs = LoadShader(_tessellationControlShader, GL_TESS_CONTROL_SHADER);
	}

	if (_tessellationEvaluationShader != nullptr)
	{
		tes = LoadShader(_tessellationEvaluationShader, GL_TESS_EVALUATION_SHADER);
	}
	
	unsigned int shaderID = CreateProgram(vs, fs, gs, tes, tcs);
	shaderMap[_shaderName] = shaderID;

	return shaderID;
}

unsigned int ShaderHandler::GetShader(string& _name)
{
	if (DoesShaderExist(_name))
	{
		return shaderMap.at(_name);
	}

	return 0;
}

unsigned int ShaderHandler::GetScreenAlignedShader()
{
	//TODO
	return 0;
}

void ShaderHandler::SetProjectionMatrix(glm::mat4 _projectionMatrix)
{
	for (auto iter = shaderMap.begin(); iter != shaderMap.end(); ++iter)
	{
		glUseProgram(iter->second);
		GLint location = glGetUniformLocation(iter->second, "Projection");
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(_projectionMatrix));
	}
}

void ShaderHandler::SetViewMatrix(glm::mat4 _viewMatrix)
{
	for (auto iter = shaderMap.begin(); iter != shaderMap.end(); ++iter)
	{
		glUseProgram(iter->second);
		GLint location = glGetUniformLocation(iter->second, "View");
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(_viewMatrix));
	}
}

void ShaderHandler::SetEyePosition(glm::vec4 _eyePosition)
{
	for (auto iter = shaderMap.begin(); iter != shaderMap.end(); ++iter)
	{
		glUseProgram(iter->second);
		GLint location = glGetUniformLocation(iter->second, "EyePos");
		glUniform4fv(location, 1, glm::value_ptr(_eyePosition));
	}
}

bool ShaderHandler::DoesShaderExist(string& _materialName)
{
	if (shaderMap.find(_materialName) != shaderMap.end())
	{
		return true;
	}

	return false;
}

unsigned int ShaderHandler::LoadShader(const char* _name, unsigned int _type)
{
	int success = GL_FALSE;

	auto i = this;

	unsigned int handle = glCreateShader(_type);
	unsigned char* source = FileToBuffer(_name);

	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

	//shader error checking
	if (success == GL_FALSE)
	{
		//get the length and create a char array == that length
		int infoLogLength = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Shader compilation has failed... \n");
		printf("%s", infoLog);
		printf("\n");

		delete[] infoLog;

		return 0;
	}

	delete[] source;

	return handle;
}

void ShaderHandler::CreateErrorShader()
{
	char* vsSource = "#version 330\n\
					 layout(location=0)in vec4 Position; \
					 uniform mat4 Projection; \
					 uniform mat4 View; \
					 uniform mat4 Model; \
					 void main() { gl_Position = Projection * View * Model * Position; }";

	char* fsSource = "#version 330\n \
					 out vec4 outColour; \
					 void main()	{ outColour = vec4(1, 0, 1, 1); }";
	
	GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vsHandle, 1, (const char**)&vsSource, 0);
	glCompileShader(vsHandle);

	glShaderSource(fsHandle, 1, (const char**)&fsSource, 0);
	glCompileShader(fsHandle);

	errorShader = glCreateProgram();
	glAttachShader(errorShader, vsHandle);
	glAttachShader(errorShader, fsHandle);
	glBindAttribLocation(errorShader, 0, "Position");
	glBindAttribLocation(errorShader, 0, "outColour");
	glLinkProgram(errorShader);

	shaderMap["errorShader"] = errorShader;
}

unsigned char* ShaderHandler::FileToBuffer(const char* _name)
{
	//open file for text reading
	FILE* file = fopen(_name, "rb"); //read binary
	if (file == nullptr)
	{
		printf("Error: Unable to open file '%s'for reading...\n", _name);
		return nullptr;
	}

	//get number of bytes in file
	fseek(file, 0, SEEK_END);
	unsigned int length = (unsigned int)ftell(file);
	

	fseek(file, 0, SEEK_SET);
	//allocate buffer and read file contents
	unsigned char* buffer = new unsigned char[length + 1];
	memset(buffer, 0xCD, length + 1);
	fread(buffer, sizeof(unsigned char), length, file);

	buffer[length] = '\0';

	fclose(file); //close the file once done reading in
	return buffer;
}

unsigned int ShaderHandler::CreateProgram(GLuint _vertShader,
	GLuint _fragShader,
	GLuint _geomShader,
	GLuint _tesselEvalShader,
	GLuint _tesselControlShader)
{
	int success = GL_FALSE;

	//create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, _vertShader);
	glAttachShader(handle, _tesselControlShader);
	glAttachShader(handle, _tesselEvalShader);
	glAttachShader(handle, _geomShader);
	glAttachShader(handle, _fragShader);

	//link the program together and log any errors
	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: failed to link shader program... \n");
		printf("%s", infoLog);
		printf("\n");
		
		delete[] infoLog;

		return 0;
	}

	return handle;
}