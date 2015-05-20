#include "Application.h"
#include "Gizmos_Fix.h"
#include "Camera.h"
#include <GLFW\glfw3.h>

#include "ShaderHandler.h"
#include "MaterialHandler.h"

Application::Application(string _name, unsigned int _width, unsigned int _height)
{
	appName = _name;
	m_camera = nullptr;

	screenWidth = _width;
	screenHeight = _height;

	totalRunTime = glfwGetTime();
	deltaTime = 0.0f;
}

Application::~Application()
{

}

static void testFunc(GLFWwindow* window, int a, int b, int c, int d)
{

}

bool Application::InitialiseOpenGL()
{
	//checking if GLFW initialised successfully
	if (glfwInit() == false)
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//creating the window
	window = glfwCreateWindow(1280, 720, "GraphicsEngine", nullptr, nullptr);

	//making sure the window was successfully created
	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, testFunc);

	Gizmos::create();

	glClearColor(0.25f, 0.25f, 0.25f, 1); //grey ;)
	glEnable(GL_DEPTH_TEST); //enables the depth buffer
	//glEnable(GL_FRONT_AND_BACK);

	return true; //all initialisation was successful
}

void Application::Run()
{
	InitialiseOpenGL();

	//initialising shader and material handler
	ShaderHandler::Init();
	MaterialHandler::Init();

	buffers = new Buffers();

	Startup();

	//checking if camera initilisationg has not worked, program will terminate if failed
	assert(m_camera != NULL && "Camera not initialized");
	assert(m_camera->GetPerspectiveSet() == true && "Camera Perspective not set");

	bool running = true;
	while (!glfwWindowShouldClose(window) && running)
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - totalRunTime;
		totalRunTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		running = Update(deltaTime);

		ShaderHandler::Get()->SetEyePosition(glm::vec4(m_camera->GetPosition(), 1));
		ShaderHandler::Get()->SetProjectionMatrix(m_camera->GetProjection());
		ShaderHandler::Get()->SetViewMatrix(m_camera->GetView());
		Render();

		Gizmos::draw(m_camera->GetProjectionView());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Shutdown();

	Gizmos::destroy();

	MaterialHandler::Deinit();
	ShaderHandler::Deinit();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::GenerateGrid(int _size)
{
	assert(_size > 0 && "Needs to be a positive size"); //program will terminate if false

	Gizmos::addTransform(glm::mat4(1));
	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	int halfSize = _size / 2;

	for (int i = 0; i < _size + 1; ++i)
	{
		Gizmos::addLine(glm::vec3(-halfSize + i, 0, halfSize),
						glm::vec3(-halfSize + i, 0, -halfSize),
						i == halfSize ? white : black);

		Gizmos::addLine(glm::vec3(halfSize, 0, -halfSize + i),
						glm::vec3(-halfSize, 0, -halfSize + i),
						i == halfSize ? white : black);
	}
}