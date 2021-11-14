#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>

#include "Application.h"
#include "World/ChunkMesh.h"

Application::Application() : m_Camera{ glm::vec3{ 0.0f, 96.0f, 0.0f }, 0.0f, 0.0f, 90.0f }
{
	frames = 0;
	time = 0;
	m_LastFrame = 0;
	m_Dt = 0;
}

void Application::init(int windowWidth, int windowHeight, const char* title)
{
	m_Window = Window{ windowWidth, windowHeight, title };
	m_World = new World{ TerrainGenerator{}, Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" } };

	ChunkMesh::createTextureAtlas("assets/textures/tex-atlas.png");
}

void Application::runMainLoop()
{
	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (!glfwWindowShouldClose(m_Window.getWindow()))
	{
		glfwSetCursorPos(m_Window.getWindow(), 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateFPS();
		handleInput();

		bool update{ false };
		if (getCurrentTimeMillis() > (updateQueues + 500))
		{
			updateQueues = getCurrentTimeMillis();
			update = true;
		}

		m_World->worldRender(m_Camera, update);

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

long Application::getCurrentTimeMillis()
{
	return static_cast<long>(glfwGetTime() * 1000);
}

void Application::handleInput()
{
	float frame = static_cast<float>(glfwGetTime());
	m_Dt = frame - m_LastFrame;
	m_LastFrame = frame;

	Keyboard& keyboard{ m_Window.getKeyboard() };
	Mouse& mouse{ m_Window.getMouse() };

	if (keyboard.isKeyDown(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(m_Window.getWindow(), true);

	if (keyboard.isKeyDown(GLFW_KEY_W))
	{
		m_Camera.handleKeyboard(Direction::Forward, 20.0f, m_Dt);

		if (keyboard.isKeyDown(GLFW_KEY_LEFT_CONTROL))
			m_Camera.handleKeyboard(Direction::Forward, 20.0f, m_Dt);
	}

	if (keyboard.isKeyDown(GLFW_KEY_A))
		m_Camera.handleKeyboard(Direction::Left, 20.0f, m_Dt);
	
	if (keyboard.isKeyDown(GLFW_KEY_S))
		m_Camera.handleKeyboard(Direction::Back, 20.0f, m_Dt);

	if (keyboard.isKeyDown(GLFW_KEY_D))
		m_Camera.handleKeyboard(Direction::Right, 20.0f, m_Dt);
	
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

	m_Camera.handleMouse(glm::vec2{ mouse.getXOffset(), mouse.getYOffset() });
}
