#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>

#include "Application.h"

Application::Application() : m_Camera{ glm::vec3{ 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 90.0f }
{

}

Application::~Application()
{
}

void Application::init(int windowWidth, int windowHeight, const char* title)
{
	m_Window = Window{ windowWidth, windowHeight, title };
	m_World = new World{ TerrainGenerator{}, Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" } };
}

void Application::runMainLoop()
{
	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	m_World->generate();

	while (!glfwWindowShouldClose(m_Window.getWindow()))
	{
		glfwSetCursorPos(m_Window.getWindow(), 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateFPS();
		handleInput();

		m_World->worldRender(m_Camera);

		glfwSwapBuffers(m_Window.getWindow());
		glfwPollEvents();
	}
}

void Application::updateFPS()
{
	++frames;
	if (getCurrentTimeMillis() > (time + 1000))
	{
		std::string title("Minecraft! | FPS: ");
		std::string fps(std::to_string(frames));
		m_Window.setTitle((title + fps).c_str());
	
		time = getCurrentTimeMillis();
		frames = 0;
	}
}

__int64 Application::getCurrentTimeMillis()
{
	static const __int64 magic = 116444736000000000;
	SYSTEMTIME st;
	GetSystemTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	__int64 time;
	memcpy(&time, &ft, sizeof(time));
	return (time - magic) / 10000;
}

void Application::handleInput()
{
	float frame = glfwGetTime();
	m_Dt = frame - m_LastFrame;
	m_LastFrame = frame;

	Keyboard& keyboard{ m_Window.getKeyboard() };
	Mouse& mouse{ m_Window.getMouse() };

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(m_Window.getWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_W))
	{
		m_Camera.handleKeyboard(Direction::Forward, 2.5, m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
			m_Camera.handleKeyboard(Direction::Forward, 20.0f, m_Dt);
	}

	if (keyboard.isKeyDown(GLFW_KEY_A))
		m_Camera.handleKeyboard(Direction::Left, 2.5, m_Dt);

	if (keyboard.isKeyDown(GLFW_KEY_S))
		m_Camera.handleKeyboard(Direction::Back, 2.5, m_Dt);

	if (keyboard.isKeyDown(GLFW_KEY_D))
		m_Camera.handleKeyboard(Direction::Right, 2.5, m_Dt);

	if (keyboard.isKeyDown(GLFW_KEY_SPACE))
	{
		m_Camera.handleKeyboard(Direction::Up, 2.5, m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
			m_Camera.handleKeyboard(Direction::Up, 20.0f, m_Dt);
	}

	if (keyboard.isKeyDown(GLFW_KEY_LEFT_SHIFT))
	{
		m_Camera.handleKeyboard(Direction::Down, 2.5, m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
			m_Camera.handleKeyboard(Direction::Down, 20.0f, m_Dt);
	}

	if (keyboard.isKeyDown(GLFW_KEY_C))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (keyboard.isKeyDown(GLFW_KEY_F))
		std::cout << "XYZ: " << m_Camera.getLocation().x << ", " << m_Camera.getLocation().y << ", " << m_Camera.getLocation().z << "\n";

	m_Camera.handleMouse(glm::vec2{ mouse.getXOffset(), mouse.getYOffset() }, m_Dt);
}
