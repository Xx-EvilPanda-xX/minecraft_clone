#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "World/World.h"
#include "Player/Player.h"
#include "Input/Keyboard.h"
#include "World/ChunkManager.h"
#include "Player/Camera.h"
#include "Render/RenderData.h"
#include "Input/EventHandler.h"
#include "Render/TextRenderer.h"

constexpr int numTextComponents{ 4 };

class Application
{
private:
	Window m_Window;
	World m_World;
	Camera m_Camera;

	RenderData m_CrossHair;
	Shader m_GuiShader;
	Shader m_TextShader;
	EventHandler m_Handler;

	TextRenderer m_TextComponents[numTextComponents];

	double m_LastFrame;
	double guiUpdateCooldown;

	long frames;
	long time;
	int currentFps;

	long doDeletePass;

	void handleInput();

	void updateFPS();

	long getCurrentTimeMillis();

	void createCrosshair();

public:
	static double s_Dt;

	Application(int windowWidth, int windowHeight, const char* title, ChunkManager& chunkManager);

	void init();

	void run();

	void renderGui();

	void updateGui();

	Window& getWindow();

	Camera& getCamera();

	World& getWorld();

	void renderCrosshair();
};


#endif