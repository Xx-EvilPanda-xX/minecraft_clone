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

class Application
{
private:
	Window m_Window;
	World m_World;
	Camera m_Camera;

	RenderData m_CrossHair;
	Shader m_GuiShader;

	double m_LastFrame;

	long frames;
	long time;

	long doDeletePass;

	void handleInput();

	void updateFPS();

	long getCurrentTimeMillis();

	void createCrosshair();

public:
	static double s_Dt;

	Application(int windowWidth, int windowHeight, const char* title, ChunkManager& chunkManager);

	void init();

	void runMainLoop();

	Window& getWindow();

	Camera& getCamera();

	World& getWorld();

	void renderCrosshair();
};


#endif