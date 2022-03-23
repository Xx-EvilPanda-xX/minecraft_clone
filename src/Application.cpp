#include <iostream>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "World/ChunkMesh.h"
#include "Render/Texture.h"
#include "Constants.h"

Application::Application(int windowWidth, int windowHeight, const char* title, ChunkManager& chunkManager)
	: m_World{ Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" }, Player{ m_Camera, chunkManager, m_Window.getKeyboard(), constants::playerReach }, chunkManager },
	m_Window{ windowWidth, windowHeight, title },
	m_Camera{ glm::dvec3{ 0.0, 96.0, 0.0 }, 0.0, 0.0, 90.0, constants::mouse_sensitivity, m_Window.getWindow() },
	m_Handler{}
{
	chunkManager.setWorld(&m_World);

	frames = 0;
	time = 0;
	m_LastFrame = 0.0;
	guiUpdateCooldown = 0.0;
}

double Application::s_Dt{};

void Application::init()
{
	m_GuiShader = Shader("assets/shaders/gui_vert.glsl", "assets/shaders/gui_frag.glsl");
	ChunkMesh::createTextureAtlas("assets/textures/tex-atlas_20.png");
	TextRenderer::createChars();
	createCrosshair();
	m_TextComponents[0] = TextRenderer{ "FPS: ", -1.0, 1.0, 0.1, 0.15, m_GuiShader };
	m_TextComponents[1] = TextRenderer{ "XYZ: ", -1.0, 0.975, 0.1, 0.15, m_GuiShader };
	m_TextComponents[2] = TextRenderer{ "Chunk Index: ", -1.0, 0.95, 0.1, 0.15, m_GuiShader };
	m_TextComponents[3] = TextRenderer{ "Selected Block: ", -1.0, 0.75, 0.1, 0.15, m_GuiShader };
	//m_TextComponents[4] = TextRenderer{ "[`]: Gravel, [1]: Grass, [2]: Stone, [3]: Dirt, [4]: CobbleStone, [5]: Wood, [6]: Leaves, [7]: Glass, [8]: Sand, [9]: Planks, [0]: DiamondBlock, [-]: Water, [=]: CraftingTable", -1.0, -0.075, 0.075, 0.15, m_GuiShader };

	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void Application::run()
{
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

		m_World.worldUpdate(m_Camera, deletePass);
		m_World.worldRender(m_Camera);
		updateGui();
		renderGui();

		glfwSwapBuffers(m_Window.getWindow());
		glfwPollEvents();
	}
}

void Application::renderGui()
{
	renderCrosshair();
	for (int i{}; i < numTextComponents; ++i)
	{
		m_TextComponents[i].render(m_Handler, m_Window);
	}
}

void Application::updateGui()
{
	if (guiUpdateCooldown <= 0.0)
	{
		m_TextComponents[0].update(std::string{ "FPS: " } + std::to_string(currentFps), -1.0, 1.0, 0.1, 0.15);

		glm::dvec3 pos{ m_Camera.getLocation() };
		m_TextComponents[1].update(std::string{ "XYZ: " } + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z), -1.0, 0.975, 0.1, 0.15);

		Vector2i chunkPos{ static_cast<int>(m_Camera.getLocation().x) / 16, static_cast<int>(m_Camera.getLocation().z) / 16 };
		m_TextComponents[2].update(std::string{ "Chunk index: " } + std::to_string(m_World.getChunkIndex(chunkPos)), -1.0, 0.95, 0.1, 0.15);

		m_TextComponents[3].update(std::string{ "Selected block: " } + m_Handler.getSelectedBlock().getName(), -1.0, 0.75, 0.1, 0.15);

		guiUpdateCooldown = 0.05;
	}

	guiUpdateCooldown -= s_Dt;
}

void Application::createCrosshair()
{
	m_CrossHair.shader = &m_GuiShader;

	Texture crosshairTex{ "assets/textures/crosshair.png", false };
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

void Application::updateFPS()
{
	++frames;
	if (getCurrentTimeMillis() > (time + 1000))
	{
		currentFps = frames;
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
	double frame = static_cast<double>(glfwGetTime());
	s_Dt = frame - m_LastFrame;
	m_LastFrame = frame;

	if (s_Dt > 0.01)
		s_Dt = 0.01;

	Keyboard& keyboard{ m_Window.getKeyboard() };
	Mouse& mouse{ m_Window.getMouse() };

	m_Handler.handleKeyboard(keyboard, *this, m_World.getPlayer());
	m_Handler.handleMouse(mouse, m_World.getPlayer());

	m_Camera.handleLook(glm::dvec2{ mouse.getXOffset(), mouse.getYOffset() });
}

void Application::renderCrosshair()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	m_GuiShader.bind();

	int width, height;
	glfwGetWindowSize(m_Window.getWindow(), &width, &height);
	double aspect{ static_cast<double>(width) / static_cast<double>(height) };
	glm::dmat4 proj{ glm::ortho(-aspect, aspect, 1.0, -1.0, -1.0, 1.0) };

	glm::dmat4 model(1.0);
	model = glm::scale(model, glm::dvec3(0.05, 0.05, 1.0));

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
