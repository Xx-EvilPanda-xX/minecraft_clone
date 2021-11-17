
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

class Application
{
private:
	Window m_Window;
	World* m_World;
	Camera m_Camera;

	float m_LastFrame;

	long frames;
	long time;

	long updateQueues;

	void handleInput();

	void updateFPS();

	long getCurrentTimeMillis();

public:
	float m_Dt;

	Application();

	void init(int windowWidth, int windowHeight, const char* title);

	void runMainLoop();

	Window& getWindow();

	Camera& getCamera();

	World* getWorld();
};


#endif