
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

	float m_Dt;
	float m_LastFrame;

	long frames;
	long time;

	void handleInput();

	void updateFPS();

	long getCurrentTimeMillis();

public:
	Application();

	~Application();

	void init(int windowWidth, int windowHeight, const char* title);

	void runMainLoop();
};


#endif