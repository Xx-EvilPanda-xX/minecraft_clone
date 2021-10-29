#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Application.h"

extern unsigned int g_MainShader{};

Application::Application() : m_World{ TerrainGenerator{} }
{
}

Application::~Application()
{
}

void Application::init(int windowWidth, int windowHeight, const char* title)
{
	m_Window = Window{ windowWidth, windowHeight, title };
	m_Keyboard = Keyboard(m_Window.getWindow());


}

void Application::runMainLoop()
{
	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);

	m_World.generate();

	while (!glfwWindowShouldClose(m_Window.getWindow()))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		m_World.worldRender();
		handleInput();

		glfwSwapBuffers(m_Window.getWindow());
		glfwPollEvents();
	}
}

void Application::handleInput()
{
	if (m_Keyboard.isKeyDown(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(m_Window.getWindow(), true);
	}
}
