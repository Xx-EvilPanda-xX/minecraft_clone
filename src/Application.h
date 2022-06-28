#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <thread>
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

	EventHandler m_Handler;
	RenderData m_CrossHair;
	Shader m_GuiShader;
	Shader m_TextShader;

	TextRenderer m_TextComponents[numTextComponents];

	std::thread m_ChunkThread;
	bool m_Running;

	double m_LastFrame;
	double m_GuiUpdateCooldown;

	long long m_Frames;
	long long m_Time;
	long long m_CurrentFps;

	void startupHelp();

	void loadConfigFromFile(std::filesystem::path path);

	void handleInput();

	void updateFPS();

	long long getCurrentTimeMillis();

	void createCrosshair();

public:
	static double s_Dt;

	Application(int windowWidth, int windowHeight, const char* title);

	void init();

	void run();

	void renderGui();

	void updateGui();

	Window& getWindow();

	World& getWorld();

	bool isRunning();

	void renderCrosshair();
};


#endif