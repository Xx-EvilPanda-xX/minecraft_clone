
#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "World/World.h"
#include "Player/Player.h"
#include "Input/Keyboard.h"
#include "World/ChunkManager.h"

class Application
{
private:
	Window m_Window;
	Keyboard m_Keyboard;
	World* m_World;

	void handleInput();

public:
	Application();

	~Application();

	void init(int windowWidth, int windowHeight, const char* title);

	void runMainLoop();
};


#endif