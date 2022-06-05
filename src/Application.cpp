#include <iostream>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "World/ChunkMesh.h"
#include "Render/Texture.h"
#include "Constants.h"

Application::Application(int windowWidth, int windowHeight, const char* title)
	: m_World{ Shader{ "assets/shaders/vert.glsl", "assets/shaders/frag.glsl" }, m_Window.getKeyboard() },
	m_Window{ windowWidth, windowHeight, title },
	m_Handler{}
{
	m_CrossHair = { 0, 0, 0, 0, 0, 0, nullptr, 0 };
	m_Frames = 0;
	m_Time = 0;
	m_LastFrame = 0.0;
	m_GuiUpdateCooldown = 0.0;
	m_CurrentFps = 0;
	m_DoDeletePass = false;
}

double Application::s_Dt{};

void Application::init()
{
	m_GuiShader = Shader("assets/shaders/gui_vert.glsl", "assets/shaders/gui_frag.glsl");
	ChunkMesh::createTextureAtlas("assets/textures/tex-atlas_27.png");
	TextRenderer::createChars();
	createCrosshair();
	m_TextComponents[0] = TextRenderer{ "FPS: ", -1.0, 1.0, 0.1, 0.15, m_GuiShader };
	m_TextComponents[1] = TextRenderer{ "XYZ: ", -1.0, 0.975, 0.1, 0.15, m_GuiShader };
	m_TextComponents[2] = TextRenderer{ "Chunk adress: ", -1.0, 0.95, 0.1, 0.15, m_GuiShader };
	m_TextComponents[3] = TextRenderer{ "Selected Block: ", -1.0, 0.75, 0.1, 0.15, m_GuiShader };

	glClearColor(0.0f, 0.4f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MIPMAP);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	if (constants::useTranslucentWater)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	startupHelp();
	std::cout << "size of app: " << sizeof(Application) << "\n";
}

void Application::run()
{
	while (!glfwWindowShouldClose(m_Window.getGlfwWindow()))
	{
		glfwSetCursorPos(m_Window.getGlfwWindow(), 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bool deletePass{ false };
		if (getCurrentTimeMillis() > (m_DoDeletePass + 500))
		{
			m_DoDeletePass = getCurrentTimeMillis();
			deletePass = true;
		}

		m_World.worldUpdate(deletePass);
		m_World.worldRender(m_Window);
		updateGui();
		renderGui();

		updateFPS();
		handleInput();

		glfwSwapBuffers(m_Window.getGlfwWindow());
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
	if (m_GuiUpdateCooldown <= 0.0)
	{
		glm::dvec3 camLoc{ m_World.getPlayer().getCamera().getLocation() };
		Vector3i iVecCamLoc{ camLoc };
		Vector2i chunkPos{ static_cast<int>(camLoc.x) / 16, static_cast<int>(camLoc.z) / 16 };

		if (iVecCamLoc.x < 0 && iVecCamLoc.x % 16 != 0)
			--chunkPos.x;

		if (iVecCamLoc.z < 0 && iVecCamLoc.z % 16 != 0)
			--chunkPos.y;

		m_TextComponents[0].update(std::string{ "FPS: " } + std::to_string(m_CurrentFps), -1.0, 1.0, 0.1, 0.15);
		m_TextComponents[1].update(std::string{ "XYZ: " } + std::to_string(camLoc.x) + ", " + std::to_string(camLoc.y) + ", " + std::to_string(camLoc.z), -1.0, 0.975, 0.1, 0.15);
		m_TextComponents[2].update(std::string{ "Chunk index: " } + std::to_string(m_World.getChunkIndex(chunkPos)), -1.0, 0.95, 0.1, 0.15);
		m_TextComponents[3].update(std::string{ "Selected block: " } + m_Handler.getSelectedBlock().getName(), -1.0, 0.75, 0.1, 0.15);

		m_GuiUpdateCooldown = 0.05;
	}

	m_GuiUpdateCooldown -= s_Dt;
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
	++m_Frames;
	if (getCurrentTimeMillis() > (m_Time + 1000))
	{
		m_CurrentFps = m_Frames;
		m_Time = getCurrentTimeMillis();
		m_Frames = 0;
	}
}

long long Application::getCurrentTimeMillis()
{
	return static_cast<long long>(glfwGetTime() * 1000);
}

void Application::startupHelp()
{
	std::cout << "\n\nCONTROLS:\n\n";
	std::cout << "Move forward: W\nMove backward: S\nMove right: D\nMove left: A\nMove up: space (fly only)\nMove down: right shift\nJump: space (gravity only)\n";
	std::cout << "Sprint: left control\nToggle flying: F\nEnable wireframe: C\n";
	std::cout << "Optifine zoom: left alt\nReload chunks: R\nQuit: ESC";
	std::cout << "\n\n===================================================\n\n";
	std::cout << "BLOCKS:\n\n";
	std::cout << "1: Grass\n2: Stone\n3: Dirt\n4: Cobble Stone\n5: Wood\n6: Palm Leaves\n7: Oak Leaves\n8: Glass\n9: Sand\n0: Planks\n";
	std::cout << "Tab + 1: Cactus\nTab + 2: Gravel\nTab + 3: Diamond Block\nTab + 4: Snow\nTab + 5: Water\nTab + 6: Crafting Table\nTab + 7: Flowers\nTab + 8: Tall Grass\nTab + 9: Shrub\nTab + 0: Rose\n\n";
}

void Application::handleInput()
{
	double frame = static_cast<double>(glfwGetTime());
	s_Dt = frame - m_LastFrame;
	m_LastFrame = frame;

	if (s_Dt > 0.25)
		s_Dt = 0.25;

	Keyboard& keyboard{ m_Window.getKeyboard() };
	Mouse& mouse{ m_Window.getMouse() };

	m_Handler.handleKeyboard(keyboard, *this);
	m_Handler.handleMouse(mouse, m_World.getPlayer());

	m_World.getPlayer().getCamera().handleLook(glm::dvec2{mouse.getXOffset(), mouse.getYOffset()});
}

void Application::renderCrosshair()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	m_GuiShader.bind();

	int width, height;
	glfwGetWindowSize(m_Window.getGlfwWindow(), &width, &height);
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

World& Application::getWorld()
{
	return m_World;
}
