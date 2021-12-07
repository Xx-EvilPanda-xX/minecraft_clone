#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "World/ChunkMesh.h"
#include "Input/EventHandler.h"
#include "Render/Texture.h"
#include "Constants.h"

Application::Application(int windowWidth, int windowHeight, const char* title, ChunkManager& chunkManager)
	: m_World{ Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" }, Player{ m_Camera, chunkManager, constants::playerReach }, chunkManager },
	m_Window{ windowWidth, windowHeight, title },
	m_Camera{ glm::vec3{ 0.0f, 96.0f, 0.0f }, 0.0f, 0.0f, 90.0f, m_Window.getWindow() }
{
	chunkManager.setWorld(&m_World);

	frames = 0;
	time = 0;
	m_LastFrame = 0;
}

float Application::s_Dt{};

void Application::init()
{
	ChunkMesh::createTextureAtlas("assets/textures/tex-atlas.png");
	createCrosshair();
}

void Application::createCrosshair()
{
	m_GuiShader = Shader("assets/shaders/gui_vert.glsl", "assets/shaders/gui_frag.glsl");
	m_CrossHair.shader = &m_GuiShader;

	Texture crosshairTex{ "assets/textures/crosshair.png" };
	m_CrossHair.texture = crosshairTex.getId();

	float vertices[]{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f };
	float tex[]{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
	int indices[]{ 1, 2, 3, 0, 2, 1 };

	unsigned int vbo, tbo, ebo, vao;
	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &tbo);
	glCreateBuffers(1, &ebo);

	m_CrossHair.vao = vao;
	m_CrossHair.vbo = vbo;
	m_CrossHair.tbo = tbo;
	m_CrossHair.ebo = ebo;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex), tex, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_CrossHair.indexCount = sizeof(indices) / sizeof(indices[0]);
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

		bool deletePass{ false };
		if (getCurrentTimeMillis() > (doDeletePass + 500))
		{
			doDeletePass = getCurrentTimeMillis();
			deletePass = true;
		}

		m_World.worldRender(m_Camera, deletePass);
		renderCrosshair();

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
	s_Dt = frame - m_LastFrame;
	m_LastFrame = frame;

	Keyboard& keyboard{ m_Window.getKeyboard() };
	Mouse& mouse{ m_Window.getMouse() };

	EventHandler::keyboardEvent(keyboard, *this, m_World.getPlayer());
	EventHandler::mouseEvent(mouse, m_World.getPlayer());

	m_Camera.handleMouse(glm::vec2{ mouse.getXOffset(), mouse.getYOffset() });
}

void Application::renderCrosshair()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	m_GuiShader.bind();

	int width, height;
	glfwGetWindowSize(m_Window.getWindow(), &width, &height);
	glm::mat4 proj(glm::perspective(glm::radians(90.0f), static_cast<float>(width) / static_cast<float>(height), 0.001f, 10.0f));

	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));

	m_GuiShader.setMat4("proj", proj);
	m_GuiShader.setMat4("model", model);

	glBindTexture(GL_TEXTURE_2D, m_CrossHair.texture);
	glBindVertexArray(m_CrossHair.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_CrossHair.ebo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawElements(GL_TRIANGLES, m_CrossHair.indexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_GuiShader.unbind();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

Window& Application::getWindow()
{
	return m_Window;
}

Camera& Application::getCamera()
{
	return m_Camera;
}

World& Application::getWorld()
{
	return m_World;
}
