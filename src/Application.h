
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

	__int64 frames;
	__int64 time;

	void handleInput();

	void updateFPS();

	__int64 getCurrentTimeMillis();

public:
	Application();

	~Application();

	void init(int windowWidth, int windowHeight, const char* title);

	void runMainLoop();
};


#endif