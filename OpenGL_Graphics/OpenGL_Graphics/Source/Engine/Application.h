#pragma once
#include <string>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//#include "AntTweakBar.h"

using namespace std;

class Camera;

class Application
{
public:

	Application(string appName, unsigned int _width = 1280, unsigned int _height = 720);
	Application() {};
	~Application();	

	//these functions to be overriden by all child objects
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;
	virtual bool Update(double _dt) = 0;
	virtual void Render() = 0;

	void Run();

protected:
	
	struct Buffers
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
	};

	//helper function which can be used by child classes
	void GenerateGrid(int _size);

	inline unsigned int GetScreenWidth() const { return screenWidth; }
	inline unsigned int GetScreenHeight() const { return screenHeight; }

	//variables for editing aspect of the world within a scene
	float editAmplitude;
	float editPersistance;

	string appName;
	GLFWwindow* window;
	Buffers* buffers;
	Camera* m_camera;

private:

	bool InitialiseOpenGL();

	unsigned int screenWidth;
	unsigned int screenHeight;

	double totalRunTime;
	double deltaTime;

};