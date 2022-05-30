#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

class Window
{
private:
	GLFWwindow* m_GlfwWindow;
	int m_Width;
	int m_Height;
	const char* m_Title;
	static Keyboard s_Keyboard;
	static Mouse s_Mouse;

	void initGlfw();
	void createWindow();
	void loadGL();

public:
	Window(int width, int height, const char* title);

	Window();

	GLFWwindow* getGlfwWindow() const;

	int getWidth() const;

	int getHeight() const;

	const char* getTitle() const;

	static Keyboard& getKeyboard();

	static Mouse& getMouse();

	void setTitle(const char* title);
};

#endif